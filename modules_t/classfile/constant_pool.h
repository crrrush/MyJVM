#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "class_reader.hpp"

namespace jvm {
namespace classfile {

class ConstantInfo; // 前向声明

class ConstantPool {
public:
    // 读取常量池 这个后面可改造为构造函数吗？
    static std::shared_ptr<ConstantPool> read_constant_pool(ClassReader& reader);

    // 按索引查找常量
    std::shared_ptr<ConstantInfo> get_constant_info(uint16_t index) const;
    // 从常量池查找字段或方法的名字和描述符
    std::pair<std::string, std::string> get_name_and_type(uint16_t index) const;
    // 从常量池查找类名
    std::string get_class_name(uint16_t index) const;

    // 从常量池查找UTF-8字符串
    std::string get_utf8(uint16_t index) const;

    // 获取常量池的大小
    uint32_t size() const;

private:
    std::vector<std::shared_ptr<ConstantInfo>> _pool;

    // ConstantPool() = default; // 私有构造函数
};


}// namespace classfile
}// namespace jvm