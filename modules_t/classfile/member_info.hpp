#pragma once

#include <vector>
#include <string>
#include <memory>

#include "classreader.hpp"
#include "constant_pool.hpp"
#include "class_reader.hpp"
#include "attribute_info.hpp"

class MemberInfo {
public:
    MemberInfo(ConstantPool& cp, uint16_t access_flags, 
               uint16_t name_index, uint16_t descriptor_index,
               std::vector<std::unique_ptr<AttributeInfo>> attributes)
        : _cp(cp), _access_flags(access_flags), 
          _name_index(name_index), _descriptor_index(descriptor_index),
          _attributes(std::move(attributes)) {}

    // 两个static函数待议，是否是这样设计？
    static std::vector<std::unique_ptr<MemberInfo>> read_members(ClassReader& reader, ConstantPool& cp) 
    {
        uint16_t member_count = reader.read_uint16();
        std::vector<std::unique_ptr<MemberInfo>> members;
        members.reserve(member_count);
        
        for (int i = 0; i < member_count; i++) 
        {
            members.push_back(read_member(reader, cp));
        }
        return members;
    }

    static std::unique_ptr<MemberInfo> read_member(ClassReader& reader, ConstantPool& cp) 
    {
        return std::make_unique<MemberInfo>(
            cp,
            reader.read_uint16(),
            reader.read_uint16(),
            reader.read_uint16(),
            AttributeInfo::read_attributes(reader, cp)
        );
    }

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

