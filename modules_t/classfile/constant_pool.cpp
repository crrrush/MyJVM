
#include "constant_pool.h"
#include "constant_info.hpp"  // 需要定义常量池项的类型

namespace jvm {
namespace classfile {

// 读取常量池 这个后面可改造为构造函数
std::shared_ptr<ConstantPool> ConstantPool::read_constant_pool(ClassReader& reader) {
    uint16_t cp_count = reader.read_uint16();
    auto cp = std::make_shared<ConstantPool>();
    cp->_pool.resize(cp_count);

    // The constant_pool table is indexed from 1 to constant_pool_count - 1
    for (uint16_t i = 1; i < cp_count; i++) {
        cp->_pool[i] = read_constant_info(reader, cp);
        // Double and Long take up two slots
        if (dynamic_cast<ConstantLongInfo*>(cp->_pool[i].get()) ||
            dynamic_cast<ConstantDoubleInfo*>(cp->_pool[i].get())) {
            i++;
        }
    }
    return cp;
}

// 按索引查找常量
std::shared_ptr<ConstantInfo> ConstantPool::get_constant_info(uint16_t index) const {
    if (index >= _pool.size() || !_pool[index] || index == 0) {
        throw std::runtime_error("Invalid constant pool index: " + std::to_string(index));
    }
    return _pool[index];
}

// 从常量池查找字段或方法的名字和描述符
std::pair<std::string, std::string> ConstantPool::get_name_and_type(uint16_t index) const {
    auto nt_info = std::dynamic_pointer_cast<ConstantNameAndTypeInfo>(get_constant_info(index));
    std::string name = get_utf8(nt_info->get_name_index());
    std::string type = get_utf8(nt_info->get_descriptor_index());
    return {name, type};
}

// 从常量池查找类名
std::string ConstantPool::get_class_name(uint16_t index) const {
    if(index >= _pool.size() || 0 == index || !_pool[index]) {
        throw std::runtime_error("Invalid class index: " + std::to_string(index));
    }
    auto class_info = std::dynamic_pointer_cast<ConstantClassInfo>(get_constant_info(index));
    return class_info->get_name();
}

// 从常量池查找UTF-8字符串
std::string ConstantPool::get_utf8(uint16_t index) const {
    if(index >= _pool.size() || 0 == index || !_pool[index]) {
        throw std::runtime_error("Invalid UTF-8 index: " + std::to_string(index));
    }
    auto utf8_info = std::dynamic_pointer_cast<ConstantUtf8Info>(get_constant_info(index));
    return utf8_info->get_string();
}

uint32_t ConstantPool::size() const {
    return _pool.size();
}
    
}// namespace classfile
} // namespace jvm

