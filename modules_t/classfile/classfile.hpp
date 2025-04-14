#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "classreader.hpp"

#include "constant_pool.hpp"
#include "member_info.hpp"
#include "attribute_info.hpp"

// 前置声明
// class ConstantPool;
// class MemberInfo;
// class AttributeInfo;

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
    const std::vector<std::shared_ptr<MemberInfo>>& fields() const { return _fields; }
    const std::vector<std::shared_ptr<MemberInfo>>& methods() const { return _methods; }
    
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
    std::vector<std::shared_ptr<MemberInfo>> _fields;
    std::vector<std::shared_ptr<MemberInfo>> _methods;
    std::vector<std::shared_ptr<AttributeInfo>> _attributes;
};


void ClassFile::read(ClassReader& reader)
{
    read_and_check_magic(reader);
    read_and_check_version(reader);
    _constant_pool = read_constant_pool(reader);
    _access_flags = reader.read_uint16();
    _this_class = reader.read_uint16();
    _super_class = reader.read_uint16();
    _interfaces = reader.read_uint16s();
    _fields = read_members(reader, _constant_pool);
    _methods = read_members(reader, _constant_pool);
    _attributes = read_attributes(reader, _constant_pool);
}


void ClassFile::read_and_check_magic(ClassReader& reader)
{
    uint32_t magic = reader.read_uint32();
    if (magic != 0xCAFEBABE) 
    {
        throw std::runtime_error("java.lang.ClassFormatError: magic!");
    }
}

void ClassFile::read_and_check_version(ClassReader& reader)
{
    _minor_version = reader.read_uint16();
    _major_version = reader.read_uint16();
    
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
