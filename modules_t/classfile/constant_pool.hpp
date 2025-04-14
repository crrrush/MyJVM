#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "classreader.hpp"
#include "constant_info.hpp"  // 需要定义常量池项的类型

class ConstantPool {
public:
    static std::shared_ptr<ConstantPool> read_constant_pool(ClassReader& reader) {
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

    std::shared_ptr<ConstantInfo> get_constant_info(uint16_t index) {
        if (index >= _pool.size() || !_pool[index]) {
            throw std::runtime_error("Invalid constant pool index: " + std::to_string(index));
        }
        return _pool[index];
    }

    std::pair<std::string, std::string> get_name_and_type(uint16_t index) {
        auto nt_info = std::dynamic_pointer_cast<ConstantNameAndTypeInfo>(get_constant_info(index));
        std::string name = get_utf8(nt_info->name_index);
        std::string type = get_utf8(nt_info->descriptor_index);
        return {name, type};
    }

    std::string get_class_name(uint16_t index) {
        auto class_info = std::dynamic_pointer_cast<ConstantClassInfo>(get_constant_info(index));
        return get_utf8(class_info->name_index);
    }

    std::string get_utf8(uint16_t index) {
        auto utf8_info = std::dynamic_pointer_cast<ConstantUtf8Info>(get_constant_info(index));
        return utf8_info->str;
    }

private:
    std::vector<std::shared_ptr<ConstantInfo>> _pool;
    // static std::shared_ptr<ConstantInfo> read_constant_info(ClassReader& reader, std::shared_ptr<ConstantPool> cp);
};
