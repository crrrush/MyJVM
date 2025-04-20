
#include "member_info.h"


namespace jvm {
namespace classfile {

MemberInfo::MemberInfo(ConstantPool& cp, uint16_t access_flags, 
        uint16_t name_index, uint16_t descriptor_index,
        std::vector<std::unique_ptr<AttributeInfo>> attributes)
        : _cp(cp), _access_flags(access_flags), 
        _name_index(name_index), _descriptor_index(descriptor_index),
        _attributes(std::move(attributes)) {}

// 两个static函数待议，是否是这样设计？
std::vector<std::unique_ptr<MemberInfo>> MemberInfo::read_members(ClassReader& reader, ConstantPool& cp) 
{
    uint16_t member_count = reader.read_uint16();
    std::vector<std::unique_ptr<MemberInfo>> members;
    members.reserve(member_count);
    LOG(INFO, "member count: %d", member_count);
    
    for (int i = 0; i < member_count; i++) 
    {
        members.push_back(read_member(reader, cp));
    }
    return members;
}

std::unique_ptr<MemberInfo> MemberInfo::read_member(ClassReader& reader, ConstantPool& cp) 
{
    uint16_t access_flags = reader.read_uint16();
    uint16_t name_index = reader.read_uint16();
    uint16_t descriptor_index = reader.read_uint16();
    LOG(INFO, "member access_flags: %x, name_index: %d, descriptor_index: %d", 
        access_flags, name_index, descriptor_index);
    std::vector<std::unique_ptr<AttributeInfo>> attributes = readAttributes(&reader, cp);
    LOG(INFO, "member attributes count: %ld", attributes.size());

    return std::make_unique<MemberInfo>(
        cp,
        access_flags,
        name_index,
        descriptor_index,
        std::move(attributes)
    );
}

// Getters
// uint16_t MemberInfo::access_flags() const { return _access_flags; }

// std::string MemberInfo::name() const { return _cp.get_utf8(_name_index); }

// std::string MemberInfo::descriptor() const { return _cp.get_utf8(_descriptor_index); }

}// namespace classfile
} // namespace jvm