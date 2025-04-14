#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "../log.hpp"
#include "../util.hpp"
#include "entry.hpp"

class ClassPath {
public:
    // 使用智能指针管理Entry对象
    // using EntryPtr = std::shared_ptr<Entry>;

    // 构造函数采用委托构造的方式
    explicit ClassPath(const std::string& jreOption = "", 
                      const std::string& cpOption = "") {
        parse(jreOption, cpOption);
    }

    // 读取类文件，返回tuple包含多个返回值
    std::tuple<std::string, EntryPtr, bool> read_class(const std::string& className) {
        std::string fullName = className + ".class";
        LOG(DEBUG, "Trying to read class: %s", fullName.c_str());

        std::string data;
        EntryPtr entry;
        bool success;
        
        if(_boot_classpath.get() == nullptr) {
            LOG(ERROR, "Boot classpath is not set!");
            throw std::runtime_error("Boot classpath is not set");
        }
        std::tie(data, entry, success) = _boot_classpath->read_class(fullName);
        if (success) {
            LOG(INFO, "Class found in boot classpath");
            return std::make_tuple(data, entry, true);
        }

        if(_ext_classpath.get() != nullptr) {
            std::tie(data, entry, success) = _ext_classpath->read_class(fullName);
            if (success) {
                LOG(INFO, "Class found in ext classpath");
                return std::make_tuple(data, entry, true);
            }    
        }    

        if(_user_classpath.get() == nullptr) {
            LOG(ERROR, "User classpath is not set!");
            throw std::runtime_error("User classpath is not set");
        }
        auto result = _user_classpath->read_class(fullName);
        if (std::get<2>(result)) {
            LOG(INFO, "Class found in user classpath");
        } else {
            LOG(ERROR, "Class not found: %s", className.c_str());
        }
        return result;
    }

    std::string to_string() const {
        return _user_classpath->to_string();
    }

private:
    // 创建Classpath实例
    void parse(const std::string& jreOption, 
        const std::string& cpOption) {
        parse_boot_and_ext_classpath(jreOption);
        parse_user_classpath(cpOption);
    }

    void parse_boot_and_ext_classpath(const std::string& jreOption) {
        std::string jdkDir = get_jre_dir(jreOption);
        LOG(INFO, "Using JDK directory: %s", jdkDir.c_str());

        _boot_classpath = EntryFactory::create(join_path(jdkDir, "classes"));
        // _ext_classpath默认为空，除非有指定，但是现在暂时不支持指定扩展类路径选项


        // For Java >= 11, lib directory is directly under JDK root
        // std::string bootPath = join_path(jdkDir, "lib");
        // std::string extPath = join_path(bootPath, "ext");

        // 这里没有指定扩展类路径就是空的
        // 启动类路径从jvm-core-libs/classes/获取
        // 这个路径中是已经解压好的启动类文件
        // 简化简易jvm的类加载逻辑


        // 这里原来是读取其中的所有的jar包，简易jvm暂不做这样的处理
        // _boot_classpath = std::make_shared<WildcardEntry>(bootPath);
        // _ext_classpath = std::make_shared<WildcardEntry>(extPath);
    }

    void parse_user_classpath(const std::string& cpOption) {
        std::string path = cpOption.empty() ? "." : cpOption;
        LOG(INFO, "Using user classpath: %s", path.c_str());
        _user_classpath = EntryFactory::create(path);
        if (!_user_classpath) {
            LOG(ERROR, "Failed to create user classpath entry: %s", path.c_str());
            throw std::runtime_error("Failed to create user classpath entry");
        }
    }

    std::string get_jre_dir(const std::string& jreOption) {
        // First try to use provided jreOption
        if (!jreOption.empty() && is_exists(jreOption)) {
            return jreOption;
        }

        // Then try to find jre directory
        if (is_exists("./jre")) {
            return "./jre";
        }

        // 自己使用jmod命令jmod extract --dir=./jvm-core-libs /pathtojmod/java.sql.jmod从jmod文件中解压出来的启动类文件
        if(is_exists("/home/crush/jvm-core-libs")) {
            return "/home/crush/jvm-core-libs";
        }

        // For Java >= 11, just use JAVA_HOME or default installation path
        // const char* javaHome = std::getenv("JAVA_HOME");
        // if (javaHome && is_exists(javaHome)) {
        //     return javaHome;
        // }

        // // Try default Ubuntu OpenJDK installation path
        // const std::string defaultPath = "/usr/lib/jvm/java-17-openjdk-amd64";
        // if (is_exists(defaultPath)) {
        //     return defaultPath;
        // }

        LOG(FATAL, "Cannot find JDK directory!");
        throw std::runtime_error("Cannot find JDK directory");
    }

    bool is_exists(const std::string& path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    std::string join_path(const std::string& path1, const std::string& path2) {
        return path1 + "/" + path2;
    }

private:
    EntryPtr _boot_classpath;
    EntryPtr _ext_classpath;
    EntryPtr _user_classpath;
};
