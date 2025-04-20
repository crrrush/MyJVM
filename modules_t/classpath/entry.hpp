#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include <dirent.h>     // 用于目录操作
#include <sys/stat.h>   // 用于文件状态
#include <limits.h>     // 用于PATH_MAX

#include <zip.h>
#include "../log.hpp"
#include "../util.hpp"


namespace jvm{
namespace classpath {

// 针对不同平台的路径分隔符
#ifdef _WIN32
    const char PATH_SEPARATOR = ';';
#else
    const char PATH_SEPARATOR = ':';
#endif

class EntryFactory;  // 前向声明

// Entry基类
class Entry {
public:
    virtual ~Entry() = default;
    
    // 读取class文件
    // 参数: className - 类名
    // 返回值: tuple<数据内容, Entry指针, 是否成功>
    virtual std::tuple<std::string, std::shared_ptr<Entry>, bool> 
    read_class(const std::string& className) = 0;
    virtual std::string to_string() const = 0;

protected:
    // 将构造函数设为protected，这样只有派生类和友元类可以访问
    Entry() = default;
    
    // 声明工厂类为友元
    friend class EntryFactory;
};

using EntryPtr = std::shared_ptr<Entry>;

// 工厂静态方法声明
class EntryFactory {
public:
    static EntryPtr create(const std::string& path);
};
    

class DirEntry : public Entry, public std::enable_shared_from_this<DirEntry> {
private:
    // 将构造函数设为私有
    explicit DirEntry(const std::string& path) {
        char realPath[PATH_MAX];
        if (realpath(path.c_str(), realPath) != nullptr) {
            _abs_dir = realPath;
        } else {
            LOG(ERROR, "Failed to resolve path: %s", path.c_str());
            throw std::runtime_error("Failed to resolve path");
        }
    }
    
    std::string _abs_dir;
    
    // 声明工厂类为友元
    friend class EntryFactory;

public:
    std::tuple<std::string, EntryPtr, bool> 
    read_class(const std::string& className) override {
        std::string fileName = _abs_dir + "/" + className;
        std::string data;
        
        if (util::util_file::read(fileName, data)) {
            return std::make_tuple(data, shared_from_this(), true);
        }
        return std::make_tuple("", nullptr, false);
    }

    std::string to_string() const override { return _abs_dir; }
};

class ZipEntry : public Entry, public std::enable_shared_from_this<ZipEntry> {
private:
    // 将构造函数设为私有
    explicit ZipEntry(const std::string& path) {
        char realPath[PATH_MAX];
        if (realpath(path.c_str(), realPath) != nullptr) {
            _abs_path = realPath;
        } else {
            LOG(ERROR, "Failed to resolve path: %s", path.c_str());
            throw std::runtime_error("Failed to resolve path");
        }
    }
    
    std::string _abs_path;
    
    // 声明工厂类为友元
    friend class EntryFactory;

public:
    std::tuple<std::string, EntryPtr, bool> 
    read_class(const std::string& className) override {
        int err = 0;
        zip* archive = zip_open(_abs_path.c_str(), ZIP_RDONLY, &err);
        if (!archive) {
            char errStr[128];
            zip_error_to_str(errStr, sizeof(errStr), err, errno);
            LOG(ERROR, "Failed to open zip file %s: %s", _abs_path.c_str(), errStr);
            return std::make_tuple("", nullptr, false);
        }

        std::unique_ptr<zip, decltype(&zip_close)> zip_guard(archive, zip_close);

        struct zip_stat st;
        zip_stat_init(&st);
        
        if (zip_stat(archive, className.c_str(), 0, &st) == 0) {
            std::string data;
            data.resize(st.size);
            
            zip_file* file = zip_fopen(archive, className.c_str(), 0);
            if (!file) {
                LOG(ERROR, "Failed to open file %s in zip", className.c_str());
                return std::make_tuple("", nullptr, false);
            }

            std::unique_ptr<zip_file, decltype(&zip_fclose)> 
                file_guard(file, zip_fclose);

            zip_int64_t readSize = zip_fread(file, &data[0], st.size);
            if (readSize == -1 || static_cast<zip_uint64_t>(readSize) != st.size) {
                LOG(ERROR, "Failed to read file %s from zip", className.c_str());
                return std::make_tuple("", nullptr, false);
            }

            return std::make_tuple(data, shared_from_this(), true);
        }

        return std::make_tuple("", nullptr, false);
    }

    std::string to_string() const override { return _abs_path; }
};

class CompositeEntry : public Entry {
private:
    // 将构造函数设为私有
    explicit CompositeEntry(const std::string& pathList) {
        std::string::size_type start = 0;
        std::string::size_type end;
        
        while ((end = pathList.find(PATH_SEPARATOR, start)) != std::string::npos) {
            if (end > start) {
                std::string path = pathList.substr(start, end - start);
                auto entry = EntryFactory::create(path);
                if (entry) {
                    _entries.push_back(entry);
                }
            }
            start = end + 1;
        }
        
        if (start < pathList.length()) {
            auto entry = EntryFactory::create(pathList.substr(start));
            if (entry) {
                _entries.push_back(entry);
            }
        }
    }
    
    std::vector<EntryPtr> _entries;
    
    // 声明工厂类为友元
    friend class EntryFactory;

public:
    std::tuple<std::string, EntryPtr, bool> 
    read_class(const std::string& className) override {
        for (const auto& entry : _entries) {
            auto [data, from, success] = entry->read_class(className);
            if (success) {
                return std::make_tuple(data, from, true);
            }
        }
        return std::make_tuple("", nullptr, false);
    }

    std::string to_string() const override {
        std::string result;
        for (const auto& entry : _entries) {
            if (!result.empty()) {
                result += PATH_SEPARATOR;
            }
            result += entry->to_string();
        }
        return result;
    }
};

class WildcardEntry : public Entry {
private:
    // 将构造函数设为私有
    explicit WildcardEntry(const std::string& path) {
        std::string baseDir = path.substr(0, path.length() - 1);
        walkDirectory(baseDir);
    }
    
    std::vector<EntryPtr> _entries;
    
    // 声明工厂类为友元
    friend class EntryFactory;

public:
    std::tuple<std::string, EntryPtr, bool> 
    read_class(const std::string& className) override {
        for (const auto& entry : _entries) {
            auto [data, from, success] = entry->read_class(className);
            if (success) {
                return std::make_tuple(data, from, true);
            }
        }
        return std::make_tuple("", nullptr, false);
    }

    std::string to_string() const override {
        std::string result;
        for (const auto& entry : _entries) {
            if (!result.empty()) {
                result += PATH_SEPARATOR;
            }
            result += entry->to_string();
        }
        return result;
    }

private:
    void walkDirectory(const std::string& baseDir) {
        // 这个函数的实现依赖于不同系统文件系统接口

        DIR* dir = opendir(baseDir.c_str());
        if (!dir) {
            LOG(ERROR, "Failed to open directory: %s", baseDir.c_str());
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name == "." || name == "..") continue;

            std::string path = baseDir + "/" + name;
            if (name.length() > 4) {
                std::string ext = name.substr(name.length() - 4);
                if (ext == ".jar" || ext == ".JAR") 
                {
                    // auto jarEntry = std::make_shared<ZipEntry>(path); // 交给工厂类创建
                    auto jarEntry = EntryFactory::create(path);
                    _entries.push_back(jarEntry);
                }
            }
        }
        closedir(dir);
    }
};


EntryPtr EntryFactory::create(const std::string& path) {
    if (path.find(PATH_SEPARATOR) != std::string::npos) {
        // return std::static_pointer_cast<Entry>(std::make_shared<CompositeEntry>(path));
        return EntryPtr(new CompositeEntry(path));
    }

    if (path.back() == '*') {
        // return std::static_pointer_cast<Entry>(std::make_shared<WildcardEntry>(path));
        return EntryPtr(new WildcardEntry(path));
    }

    if (path.length() > 4) {
        std::string ext = path.substr(path.length() - 4);
        if (ext == ".jar" || ext == ".JAR" ||
            ext == ".zip" || ext == ".ZIP") 
        {
            // return std::static_pointer_cast<Entry>(std::make_shared<ZipEntry>(path));
            // return EntryPtr(new ZipEntry(path));
            return std::static_pointer_cast<Entry>(std::shared_ptr<ZipEntry>(new ZipEntry(path)));
        }
    }

    // return std::static_pointer_cast<Entry>(std::make_shared<DirEntry>(path));
    return EntryPtr(new DirEntry(path));
}

} // namespace classpath
} // namespace jvm
