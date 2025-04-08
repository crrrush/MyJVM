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

// 针对不同平台的路径分隔符
#ifdef _WIN32
    const char PATH_SEPARATOR = ';';
#else
    const char PATH_SEPARATOR = ':';
#endif

// Entry类是所有类路径项的基类
class Entry {
public:
    virtual ~Entry() = default;
    
    // 读取class文件
    // 参数: className - 类名
    // 返回值: tuple<数据内容, Entry指针, 是否成功>
    virtual std::tuple<std::string, std::shared_ptr<Entry>, bool> 
    readClass(const std::string& className) = 0;
    
    // 返回Entry的字符串表示
    virtual std::string toString() const = 0;
};

using EntryPtr = std::shared_ptr<Entry>;
// 根据路径创建相应的Entry实例
EntryPtr createEntry(const std::string& path);


// 目录形式的类路径项
class DirEntry : public Entry, public std::enable_shared_from_this<DirEntry> {
public:
    // 构造函数，将路径转换为绝对路径
    explicit DirEntry(const std::string& path) {
        char realPath[PATH_MAX];
        if (realpath(path.c_str(), realPath) != nullptr) {
            _abs_dir = realPath;
        } else {
            LOG(ERROR, "Failed to resolve path: %s", path.c_str());
            throw std::runtime_error("Failed to resolve path");
        }
    }

    // 从目录中读取class文件
    std::tuple<std::string, EntryPtr, bool> 
    readClass(const std::string& className) override {
        std::string fileName = _abs_dir + "/" + className;
        std::string data;
        
        if (util::util_file::read(fileName, data)) {
            return std::make_tuple(data, shared_from_this(), true);
        }
        return std::make_tuple("", nullptr, false);
    }

    // 返回目录的绝对路径
    std::string toString() const override {
        return _abs_dir;
    }

private:
    std::string _abs_dir;  // 绝对路径
};


// ZIP/JAR文件形式的类路径项
class ZipEntry : public Entry, public std::enable_shared_from_this<ZipEntry> {
public:
    // 构造函数，将ZIP文件路径转换为绝对路径
    explicit ZipEntry(const std::string& path) {
        char realPath[PATH_MAX];
        if (realpath(path.c_str(), realPath) != nullptr) {
            _abs_path = realPath;
        } else {
            LOG(ERROR, "Failed to resolve path: %s", path.c_str());
            throw std::runtime_error("Failed to resolve path");
        }
    }

    // 从ZIP文件中读取class文件
    std::tuple<std::string, EntryPtr, bool> 
    readClass(const std::string& className) override {
        int err = 0;
        zip* archive = zip_open(_abs_path.c_str(), ZIP_RDONLY, &err);
        if (!archive) {
            char errStr[128];
            zip_error_to_str(errStr, sizeof(errStr), err, errno);
            LOG(ERROR, "Failed to open zip file %s: %s", _abs_path.c_str(), errStr);
            return std::make_tuple("", nullptr, false);
        }

        // 使用 RAII 确保 zip 文件被正确关闭
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

            // 使用 RAII 确保 zip_file 被正确关闭
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

    // 返回ZIP文件的绝对路径
    std::string toString() const override {
        return _abs_path;
    }

private:
    std::string _abs_path;  // ZIP文件的绝对路径
};


// 组合形式的类路径项，处理多个路径
class CompositeEntry : public Entry {
public:
    // 构造函数，解析路径列表并创建对应的Entry
    explicit CompositeEntry(const std::string& pathList) {
        std::string::size_type start = 0;
        std::string::size_type end;
        
        while ((end = pathList.find(PATH_SEPARATOR, start)) != std::string::npos) {
            if (end > start) {
                std::string path = pathList.substr(start, end - start);
                auto entry = createEntry(path);
                if (entry) {
                    _entries.push_back(entry);
                }
            }
            start = end + 1;
        }
        
        if (start < pathList.length()) {
            auto entry = createEntry(pathList.substr(start));
            if (entry) {
                _entries.push_back(entry);
            }
        }
    }

    // 依次从所有Entry中查找并读取class文件
    std::tuple<std::string, EntryPtr, bool> 
    readClass(const std::string& className) override {
        for (const auto& entry : _entries) {
            auto [data, from, success] = entry->readClass(className);
            if (success) {
                return std::make_tuple(data, from, true);
            }
        }
        return std::make_tuple("", nullptr, false);
    }

    // 返回所有Entry的字符串表示，以路径分隔符连接
    std::string toString() const override {
        std::string result;
        for (const auto& entry : _entries) {
            if (!result.empty()) {
                result += PATH_SEPARATOR;
            }
            result += entry->toString();
        }
        return result;
    }

private:
    std::vector<EntryPtr> _entries;  // Entry列表
};

// 通配符形式的类路径项，处理包含通配符(*)的路径
class WildcardEntry : public Entry {
public:
    // 构造函数，扫描基础目录下的所有JAR文件
    explicit WildcardEntry(const std::string& path) {
        std::string baseDir = path.substr(0, path.length() - 1); // remove *
        
		// 这里需要根据不同的操作系统做出适配
        // 使用文件系统遍历目录并查找JAR文件
        walkDirectory(baseDir);
    }

    // 依次从所有JAR文件中查找并读取class文件
    std::tuple<std::string, EntryPtr, bool> 
    readClass(const std::string& className) override {
        for (const auto& entry : _entries) {
            auto [data, from, success] = entry->readClass(className);
            if (success) {
                return std::make_tuple(data, from, true);
            }
        }
        return std::make_tuple("", nullptr, false);
    }

    // 返回所有JAR文件的路径，以路径分隔符连接
    std::string toString() const override {
        std::string result;
        for (const auto& entry : _entries) {
            if (!result.empty()) {
                result += PATH_SEPARATOR;
            }
            result += entry->toString();
        }
        return result;
    }

private:
    // 遍历目录，查找JAR文件并创建对应的ZipEntry
    void walkDirectory(const std::string& baseDir) {
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
                if (ext == ".jar" || ext == ".JAR") {
                    auto jarEntry = std::make_shared<ZipEntry>(path);
                    _entries.push_back(jarEntry);
                }
            }
        }
        closedir(dir);
    }

    std::vector<EntryPtr> _entries;  // ZipEntry列表
};

// // EmptyEntry class for Java 9+ where ext classpath is not used
// class EmptyEntry : public Entry {
// public:
//     std::tuple<std::string, std::shared_ptr<Entry>, bool> readClass(const std::string&) override {
//         return std::make_tuple("", nullptr, false);
//     }

//     std::string toString() const override {
//         return "";
//     }
// };


// 根据路径创建相应类型的Entry
// 规则：
// 1. 包含路径分隔符 -> CompositeEntry
// 2. 以*结尾 -> WildcardEntry
// 3. 以.jar/.JAR/.zip/.ZIP结尾 -> ZipEntry
// 4. 其他 -> DirEntry
EntryPtr createEntry(const std::string& path) {
    if (path.find(PATH_SEPARATOR) != std::string::npos) {
        return std::make_shared<CompositeEntry>(path);
    }

    if (path.back() == '*') {
        return std::make_shared<WildcardEntry>(path);
    }

    if (path.length() > 4) {
        std::string ext = path.substr(path.length() - 4);
        if (ext == ".jar" || ext == ".JAR" ||
            ext == ".zip" || ext == ".ZIP") {
            return std::make_shared<ZipEntry>(path);
        }
    }

    return std::make_shared<DirEntry>(path);
}
