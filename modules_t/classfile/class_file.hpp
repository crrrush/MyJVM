#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "class_reader.hpp"

#include "constant_pool.h"
#include "member_info.h"


namespace jvm {
namespace classfile {

class ClassFile {
public:
    // 静态工厂方法，解析类文件数据
    static std::tuple<std::shared_ptr<ClassFile>, bool> parse(const std::vector<uint8_t>& class_data) {
        try {
            ClassReader reader(class_data);
            auto cf = std::make_shared<ClassFile>();
            cf->read(reader);
            return std::make_tuple(cf, true);
        } catch (const std::exception& e) {
            LOG(ERROR, "Parse class file failed: %s", e.what());
            return std::make_tuple(nullptr, false);
        }
    }

    // Getters
    uint16_t minor_version() const { return _minor_version; }
    uint16_t major_version() const { return _major_version; }
    const ConstantPool& constant_pool() const { return *_constant_pool; }
    uint16_t access_flags() const { return _access_flags; }
    const std::vector<std::unique_ptr<MemberInfo>>& fields() const { return _fields; }
    const std::vector<std::unique_ptr<MemberInfo>>& methods() const { return _methods; }
    
    // 获取类名、父类名和接口名
    std::string class_name() const;
    std::string super_class_name() const;
    std::vector<std::string> interface_names() const;

private:
    void read(ClassReader& reader);
    void read_and_check_magic(ClassReader& reader);
    void read_and_check_version(ClassReader& reader);

private:
    uint16_t _minor_version;
    uint16_t _major_version;
    std::shared_ptr<ConstantPool> _constant_pool;
    uint16_t _access_flags;
    uint16_t _this_class;
    uint16_t _super_class;
    std::vector<uint16_t> _interfaces;
    std::vector<std::unique_ptr<MemberInfo>> _fields;
    std::vector<std::unique_ptr<MemberInfo>> _methods;
    std::vector<std::unique_ptr<AttributeInfo>> _attributes;
};


void ClassFile::read(ClassReader& reader)
{
    read_and_check_magic(reader);
    read_and_check_version(reader);
    _constant_pool = ConstantPool::read_constant_pool(reader);
    LOG(INFO, "constant pool count: %d", _constant_pool->size());

    _access_flags = reader.read_uint16();
    LOG(INFO, "access flags: 0x%X", _access_flags);

    _this_class = reader.read_uint16();
    LOG(INFO, "this class: %d", _this_class);

    _super_class = reader.read_uint16();
    LOG(INFO, "super class: %d", _super_class);

    _interfaces = reader.read_uint16s();
    LOG(INFO, "interfaces count: %ld", _interfaces.size());

    _fields = MemberInfo::read_members(reader, (*_constant_pool));
    LOG(INFO, "fields count: %ld", _fields.size());

    _methods = MemberInfo::read_members(reader, (*_constant_pool));
    LOG(INFO, "methods count: %ld", _methods.size());
    
    _attributes = readAttributes(&reader, (*_constant_pool));
    LOG(INFO, "attributes count: %ld", _attributes.size());
}


void ClassFile::read_and_check_magic(ClassReader& reader)
{
    uint32_t magic = reader.read_uint32();
    LOG(INFO, "magic: 0x%X", magic);
    if (magic != 0xCAFEBABE)
    {
        LOG(ERROR, "Invalid class file magic number: 0x%X", magic);
        throw std::runtime_error("java.lang.ClassFormatError: magic!");
    }
}

void ClassFile::read_and_check_version(ClassReader& reader)
{
    _minor_version = reader.read_uint16();
    _major_version = reader.read_uint16();
    LOG(INFO, "version: %d.%d", _major_version, _minor_version);
    
    switch (_major_version) {
        case 45:
            return;
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
            if (_minor_version == 0) {
                return;
            }
    }
    
    LOG(ERROR, "Unsupported class version: %d.%d", _major_version, _minor_version);
    throw std::runtime_error("java.lang.UnsupportedClassVersionError!");
}

std::string ClassFile::class_name() const
{
    return _constant_pool->get_class_name(_this_class);
}

std::string ClassFile::super_class_name() const
{
    if (_super_class > 0) {
        return _constant_pool->get_class_name(_super_class);
    }
    return "";
}

std::vector<std::string> ClassFile::interface_names() const
{
    std::vector<std::string> names;
    names.reserve(_interfaces.size());
    for (uint16_t index : _interfaces) {
        names.push_back(_constant_pool->get_class_name(index));
    }
    return names;
}

} // namespace classfile
} // namespace jvm
