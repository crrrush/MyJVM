#pragma once

#include <memory>
#include <vector>
#include "../util.hpp"  // 工具类头文件
#include "class_reader.hpp"
#include "constant_pool.h"  // 常量池头文件

namespace jvm {
namespace classfile {

class ConstantPool; // 前向声明

// 常量池标签
enum class CONSTANT_TAG : uint8_t {
    UTF8               = 1,
    INTEGER           = 3,
    FLOAT            = 4,
    LONG             = 5,
    DOUBLE           = 6,
    CLASS            = 7,
    STRING           = 8,
    FIELDREF         = 9,
    METHODREF        = 10,
    INTERFACE_METHODREF = 11,
    NAME_AND_TYPE    = 12,
    METHOD_HANDLE    = 15,
    METHOD_TYPE      = 16,
    INVOKE_DYNAMIC   = 18
};


// 常量信息基类
class ConstantInfo {
public:
    virtual ~ConstantInfo() = default;
    virtual void read_info(ClassReader& reader) = 0;

protected:
    // explicit ConstantInfo(std::shared_ptr<ConstantPool> cp) : constant_pool(cp) {}
    // std::shared_ptr<ConstantPool> constant_pool;
};

/////////////////// utf-8 constants /////////////////////////

// UTF8常量
class ConstantUtf8Info : public ConstantInfo {
public:
    // explicit ConstantUtf8Info(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        uint16_t length = reader.read_uint16();
        std::vector<uint8_t> bytes = reader.read_bytes(length);
        // 这里的字符串是以MUTF-8编码的
        // 需要转换为标准UTF-8编码
        _str = util::util_mutf8::decode(bytes);
    }
    std::string get_string() const {
        return _str;
    }

private:

    std::string _str;
};

/////////////////// number constants /////////////////////////

// 整数常量
class ConstantIntegerInfo : public ConstantInfo {
public:
    // explicit ConstantIntegerInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        uint32_t bytes = reader.read_uint32();
        _val = *reinterpret_cast<int32_t*>(&bytes);
    }
    int32_t _val;
};

// 浮点数常量
class ConstantFloatInfo : public ConstantInfo {
public:
    // explicit ConstantFloatInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        uint32_t bytes = reader.read_uint32();
        _val = *reinterpret_cast<float*>(&bytes);
    }
    float _val;
};

// 长整型常量
class ConstantLongInfo : public ConstantInfo {
public:
    // explicit ConstantLongInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        uint64_t bytes = reader.read_uint64();
        _val = *reinterpret_cast<int64_t*>(&bytes);
        // 读取8字节的整数常量时，常量池中会占用两个索引位置
    }
    uint64_t _val;
};

// 双精度浮点常量
class ConstantDoubleInfo : public ConstantInfo {
public:
    // explicit ConstantDoubleInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        uint64_t bytes = reader.read_uint64();
        _val = *reinterpret_cast<double*>(&bytes);
    }
    double _val;
};

// 类常量
class ConstantClassInfo : public ConstantInfo {
public:
    explicit ConstantClassInfo(std::shared_ptr<ConstantPool> cp) :_cp(cp) {}
    void read_info(ClassReader& reader) override {
        _name_index = reader.read_uint16();
    }

    std::string get_name() {
        return _cp->get_utf8(_name_index);
    }

private:
    std::shared_ptr<ConstantPool> _cp;
    uint16_t _name_index;
};

/////////////////// string constants /////////////////////////

// 字符串常量
class ConstantStringInfo : public ConstantInfo {
public:
    // explicit ConstantStringInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    explicit ConstantStringInfo(std::shared_ptr<ConstantPool> cp) :_cp(cp) {}
    void read_info(ClassReader& reader) override {
        _string_index = reader.read_uint16();
    }

    std::string get_string() {
        return _cp->get_utf8(_string_index);
    }

    std::shared_ptr<ConstantPool> _cp;
    uint16_t _string_index;
};

/////////////////// 字段或方法 name and type constants /////////////////////////

// 名称和类型
class ConstantNameAndTypeInfo : public ConstantInfo {
public:
    // explicit ConstantNameAndTypeInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        _name_index = reader.read_uint16();
        _descriptor_index = reader.read_uint16();
    }

    uint16_t get_name_index() const {
        return _name_index;
    }
    uint16_t get_descriptor_index() const {
        return _descriptor_index;
    }

private:
    uint16_t _name_index;
    uint16_t _descriptor_index;
};

/////////////////// 字段符号引用、普通（非接口）方法符号引用和接口方法符号引用 member ref /////////////////////////

// 成员引用基类
class ConstantMemberRefInfo : public ConstantInfo {
public:
    explicit ConstantMemberRefInfo(std::shared_ptr<ConstantPool> cp) : _cp(cp) {}
    void read_info(ClassReader& reader) override {
        _class_index = reader.read_uint16();
        _name_and_type_index = reader.read_uint16();
    }

    std::string get_class_name() {
        return _cp->get_class_name(_class_index);
    }

    std::pair<std::string, std::string> get_name_and_type() {
        return _cp->get_name_and_type(_name_and_type_index);
    }

    std::shared_ptr<ConstantPool> _cp;
    uint16_t _class_index;
    uint16_t _name_and_type_index;
};

// 字段符号引用
class ConstantFieldRefInfo : public ConstantMemberRefInfo {
public:
    explicit ConstantFieldRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantMemberRefInfo(cp) {}
};

// 普通（非接口）方法符号引用
class ConstantMethodRefInfo : public ConstantMemberRefInfo {
public:
    explicit ConstantMethodRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantMemberRefInfo(cp) {}
};

// 接口方法符号引用
class ConstantInterfaceMethodRefInfo : public ConstantMemberRefInfo {
public:
    explicit ConstantInterfaceMethodRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantMemberRefInfo(cp) {}
};

// MethodHandle常量
class ConstantMethodHandleInfo : public ConstantInfo {
public:
    void read_info(ClassReader& reader) override {
        _reference_kind = reader.read_uint8();
        _reference_index = reader.read_uint16();
    }
    uint8_t _reference_kind;
    uint16_t _reference_index;
};

// MethodType常量
class ConstantMethodTypeInfo : public ConstantInfo {
public:
    void read_info(ClassReader& reader) override {
        _descriptor_index = reader.read_uint16();
    }
    uint16_t _descriptor_index;
};

// InvokeDynamic常量
class ConstantInvokeDynamicInfo : public ConstantInfo {
public:
    void read_info(ClassReader& reader) override {
        _bootstrap_method_attr_index = reader.read_uint16();
        _name_and_type_index = reader.read_uint16();
    }
    uint16_t _bootstrap_method_attr_index;
    uint16_t _name_and_type_index;
};

// 工厂函数 - 创建常量信息对象
inline std::shared_ptr<ConstantInfo> new_constant_info(uint8_t tag, std::shared_ptr<ConstantPool> cp) {
    switch (static_cast<CONSTANT_TAG>(tag)) {
        case CONSTANT_TAG::INTEGER:
            return std::make_shared<ConstantIntegerInfo>();
        case CONSTANT_TAG::FLOAT:
            return std::make_shared<ConstantFloatInfo>();
        case CONSTANT_TAG::LONG:
            return std::make_shared<ConstantLongInfo>();
        case CONSTANT_TAG::DOUBLE:
            return std::make_shared<ConstantDoubleInfo>();
        case CONSTANT_TAG::UTF8:
            return std::make_shared<ConstantUtf8Info>();
        case CONSTANT_TAG::STRING:
            return std::make_shared<ConstantStringInfo>(cp);
        case CONSTANT_TAG::CLASS:
            return std::make_shared<ConstantClassInfo>(cp);
        case CONSTANT_TAG::FIELDREF:
            return std::make_shared<ConstantFieldRefInfo>(cp);
        case CONSTANT_TAG::METHODREF:
            return std::make_shared<ConstantMethodRefInfo>(cp);
        case CONSTANT_TAG::INTERFACE_METHODREF:
            return std::make_shared<ConstantInterfaceMethodRefInfo>(cp);
        case CONSTANT_TAG::NAME_AND_TYPE:
            return std::make_shared<ConstantNameAndTypeInfo>();
        case CONSTANT_TAG::METHOD_TYPE:
            return std::make_shared<ConstantMethodTypeInfo>();
        case CONSTANT_TAG::METHOD_HANDLE:
            return std::make_shared<ConstantMethodHandleInfo>();
        case CONSTANT_TAG::INVOKE_DYNAMIC:
            return std::make_shared<ConstantInvokeDynamicInfo>();
        default:
            throw std::runtime_error("java.lang.ClassFormatError: constant pool tag!");
    }
    return nullptr;
}

// 读取常量信息 - 包装工厂方法
inline std::shared_ptr<ConstantInfo> read_constant_info(ClassReader& reader, std::shared_ptr<ConstantPool> cp) {
    uint8_t tag = reader.read_uint8();
    auto c = new_constant_info(tag, cp);
    c->read_info(reader);
    return c;
}

} // namespace classfile
} // namespace jvm
