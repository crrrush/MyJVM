#pragma once

#include "classreader.hpp"
#include <memory>

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

class ConstantPool; // 前向声明

// 常量信息基类
class ConstantInfo {
public:
    virtual ~ConstantInfo() = default;
    virtual void read_info(ClassReader& reader) = 0;

protected:
    explicit ConstantInfo(std::shared_ptr<ConstantPool> cp) : constant_pool(cp) {}
    std::shared_ptr<ConstantPool> constant_pool;
};      

// UTF8常量
class ConstantUtf8Info : public ConstantInfo {
public:
    explicit ConstantUtf8Info(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        uint16_t length = reader.read_uint16();
        str = std::string(reinterpret_cast<char*>(&reader.read_bytes(length)[0]), length);
    }
    std::string str;
};

// 整数常量
class ConstantIntegerInfo : public ConstantInfo {
public:
    explicit ConstantIntegerInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        val = reader.read_uint32();
    }
    uint32_t val;
};

// 浮点数常量
class ConstantFloatInfo : public ConstantInfo {
public:
    explicit ConstantFloatInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        val = *reinterpret_cast<float*>(&reader.read_uint32());
    }
    float val;
};

// 长整型常量
class ConstantLongInfo : public ConstantInfo {
public:
    explicit ConstantLongInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        val = reader.read_uint64();
    }
    uint64_t val;
};

// 双精度浮点常量
class ConstantDoubleInfo : public ConstantInfo {
public:
    explicit ConstantDoubleInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        val = *reinterpret_cast<double*>(&reader.read_uint64());
    }
    double val;
};

// 类常量
class ConstantClassInfo : public ConstantInfo {
public:
    explicit ConstantClassInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        name_index = reader.read_uint16();
    }
    uint16_t name_index;
};

// 字符串常量
class ConstantStringInfo : public ConstantInfo {
public:
    explicit ConstantStringInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        string_index = reader.read_uint16();
    }
    uint16_t string_index;
};

// 成员引用基类
class ConstantMemberRefInfo : public ConstantInfo {
public:
    explicit ConstantMemberRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        class_index = reader.read_uint16();
        name_and_type_index = reader.read_uint16();
    }
    uint16_t class_index;
    uint16_t name_and_type_index;
};

// 字段引用
class ConstantFieldRefInfo : public ConstantMemberRefInfo {
public:
    explicit ConstantFieldRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantMemberRefInfo(cp) {}
};

// 方法引用
class ConstantMethodRefInfo : public ConstantMemberRefInfo {
public:
    explicit ConstantMethodRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantMemberRefInfo(cp) {}
};

// 接口方法引用
class ConstantInterfaceMethodRefInfo : public ConstantMemberRefInfo {
public:
    explicit ConstantInterfaceMethodRefInfo(std::shared_ptr<ConstantPool> cp) : ConstantMemberRefInfo(cp) {}
};

// 名称和类型
class ConstantNameAndTypeInfo : public ConstantInfo {
public:
    explicit ConstantNameAndTypeInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        name_index = reader.read_uint16();
        descriptor_index = reader.read_uint16();
    }
    uint16_t name_index;
    uint16_t descriptor_index;
};

// MethodHandle常量
class ConstantMethodHandleInfo : public ConstantInfo {
public:
    explicit ConstantMethodHandleInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        reference_kind = reader.read_uint8();
        reference_index = reader.read_uint16();
    }
    uint8_t reference_kind;
    uint16_t reference_index;
};

// MethodType常量
class ConstantMethodTypeInfo : public ConstantInfo {
public:
    explicit ConstantMethodTypeInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        descriptor_index = reader.read_uint16();
    }
    uint16_t descriptor_index;
};

// InvokeDynamic常量
class ConstantInvokeDynamicInfo : public ConstantInfo {
public:
    explicit ConstantInvokeDynamicInfo(std::shared_ptr<ConstantPool> cp) : ConstantInfo(cp) {}
    void read_info(ClassReader& reader) override {
        bootstrap_method_attr_index = reader.read_uint16();
        name_and_type_index = reader.read_uint16();
    }
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
};

// 工厂函数 - 创建常量信息对象
inline std::shared_ptr<ConstantInfo> new_constant_info(uint8_t tag, std::shared_ptr<ConstantPool> cp) {
    switch (static_cast<CONSTANT_TAG>(tag)) {
        case CONSTANT_TAG::INTEGER:
            return std::make_shared<ConstantIntegerInfo>(cp);
        case CONSTANT_TAG::FLOAT:
            return std::make_shared<ConstantFloatInfo>(cp);
        case CONSTANT_TAG::LONG:
            return std::make_shared<ConstantLongInfo>(cp);
        case CONSTANT_TAG::DOUBLE:
            return std::make_shared<ConstantDoubleInfo>(cp);
        case CONSTANT_TAG::UTF8:
            return std::make_shared<ConstantUtf8Info>(cp);
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
            return std::make_shared<ConstantNameAndTypeInfo>(cp);
        case CONSTANT_TAG::METHOD_TYPE:
            return std::make_shared<ConstantMethodTypeInfo>(cp);
        case CONSTANT_TAG::METHOD_HANDLE:
            return std::make_shared<ConstantMethodHandleInfo>(cp);
        case CONSTANT_TAG::INVOKE_DYNAMIC:
            return std::make_shared<ConstantInvokeDynamicInfo>(cp);
        default:
            throw std::runtime_error("java.lang.ClassFormatError: constant pool tag!");
    }
}

// 读取常量信息 - 包装工厂方法
inline std::shared_ptr<ConstantInfo> read_constant_info(ClassReader& reader, std::shared_ptr<ConstantPool> cp) {
    uint8_t tag = reader.read_uint8();
    auto c = new_constant_info(tag, cp);
    c->read_info(reader);
    return c;
}
