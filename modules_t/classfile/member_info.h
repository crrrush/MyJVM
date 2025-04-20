#pragma once

#include <vector>
#include <string>
#include <memory>

#include "class_reader.hpp"
#include "constant_pool.h"
#include "attribute_info.hpp"

namespace jvm {
namespace classfile {

class MemberInfo {
public:
    MemberInfo(ConstantPool& cp, uint16_t access_flags, 
               uint16_t name_index, uint16_t descriptor_index,
               std::vector<std::unique_ptr<AttributeInfo>> attributes);

    // 两个static函数待议，是否是这样设计？
    static std::vector<std::unique_ptr<MemberInfo>> read_members(ClassReader& reader, ConstantPool& cp);

    static std::unique_ptr<MemberInfo> read_member(ClassReader& reader, ConstantPool& cp);

    // Getters
    uint16_t access_flags() const { return _access_flags; }
    std::string name() const { return _cp.get_utf8(_name_index); }
    std::string descriptor() const { return _cp.get_utf8(_descriptor_index); }

private:
    ConstantPool& _cp;
    uint16_t _access_flags;
    uint16_t _name_index;
    uint16_t _descriptor_index;
    std::vector<std::unique_ptr<AttributeInfo>> _attributes;
};

}// namespace classfile
}// namespace jvm
