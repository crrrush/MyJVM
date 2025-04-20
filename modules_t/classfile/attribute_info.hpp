#pragma once

#include <string>
#include <vector>
#include <memory>
#include "class_reader.hpp"
#include "constant_pool.h"
// #include "util.hpp"  // 工具类头文件
// #include "../log.hpp"  // 自定义日志模块（需确保项目中有该头文件）

namespace jvm {
namespace classfile {

class ConstantPool; // 前向声明

// Abstract base class for attribute info
class AttributeInfo {
public:
    virtual ~AttributeInfo() = default;
    virtual void readInfo(ClassReader* reader) = 0;
};

std::vector<std::unique_ptr<AttributeInfo>> readAttributes(ClassReader* reader, const ConstantPool& cp);
std::unique_ptr<AttributeInfo> readAttribute(ClassReader* reader, const ConstantPool& cp);
std::unique_ptr<AttributeInfo> newAttributeInfo(const std::string& attrName, uint32_t attrLen, const ConstantPool& cp);

class UnparsedAttribute : public AttributeInfo {
private:
    std::string _attrName;
    uint32_t _attrLength;
    std::vector<uint8_t> _info;

public:
    UnparsedAttribute(const std::string& name, uint32_t length) 
        : _attrName(name), _attrLength(length) {}
    
    void readInfo(ClassReader* reader) override {
        LOG(INFO, "Unparsed attribute: %s, length: %d", _attrName.c_str(), _attrLength);
        _info = reader->read_bytes(_attrLength);
    }

    const std::vector<uint8_t>& getInfo() const {
        return _info;
    }
};

/////////////////// Attribute classes ///////////////////////


///// Deprecated 和 Synthetic 是标记属性，
class MarkerAttribute : public AttributeInfo {
public:
    virtual ~MarkerAttribute() = default;
    void readInfo(ClassReader* reader) override {
        // read nothing
    }
};

class DeprecatedAttribute : public MarkerAttribute {
public:
    virtual ~DeprecatedAttribute() = default;
};

class SyntheticAttribute : public MarkerAttribute {
public:
    virtual ~SyntheticAttribute() = default;
};

///// SourceFileAttribute 是一个特殊的属性，用于指定源文件名
class SourceFileAttribute : public AttributeInfo {
private:
    const ConstantPool& _cp;
    uint16_t _sourceFileIndex;

public:
    explicit SourceFileAttribute(const ConstantPool& cp) : _cp(cp) {}
    virtual ~SourceFileAttribute() = default;

    void readInfo(ClassReader* reader) override {
        _sourceFileIndex = reader->read_uint16();
    }

    std::string getFileName() const {
        return _cp.get_utf8(_sourceFileIndex);
    }
};

///// ConstantValueAttribute 用于指定常量值
class ConstantValueAttribute : public AttributeInfo {
private:
    uint16_t _constantValueIndex;

public:
    virtual ~ConstantValueAttribute() = default;

    void readInfo(ClassReader* reader) override {
        _constantValueIndex = reader->read_uint16();
    }

    uint16_t getConstantValueIndex() const {
        return _constantValueIndex;
    }
};

///// CodeAttribute 存储字节码等方法相关信息
class ExceptionTableEntry {
private:
    uint16_t _startPc;
    uint16_t _endPc;
    uint16_t _handlerPc;
    uint16_t _catchType;

public:
    ExceptionTableEntry(uint16_t startPc, uint16_t endPc, 
                        uint16_t handlerPc, uint16_t catchType)
        : _startPc(startPc), _endPc(endPc),
            _handlerPc(handlerPc), _catchType(catchType) {}

    uint16_t getStartPc() const { return _startPc; }
    uint16_t getEndPc() const { return _endPc; }
    uint16_t getHandlerPc() const { return _handlerPc; }
    uint16_t getCatchType() const { return _catchType; }
};

class CodeAttribute : public AttributeInfo {
private:
    const ConstantPool& _cp;
    uint16_t _maxStack;
    uint16_t _maxLocals;
    std::vector<uint8_t> _code;
    std::vector<std::unique_ptr<ExceptionTableEntry>> _exceptionTable;
    std::vector<std::unique_ptr<AttributeInfo>> _attributes;

    std::vector<std::unique_ptr<ExceptionTableEntry>> readExceptionTable(ClassReader* reader);

public:
    explicit CodeAttribute(const ConstantPool& cp) : _cp(cp) {}
    virtual ~CodeAttribute() = default;

    void readInfo(ClassReader* reader) override;

    uint16_t getMaxStack() const { return _maxStack; }
    uint16_t getMaxLocals() const { return _maxLocals; }
    const std::vector<uint8_t>& getCode() const { return _code; }
    const std::vector<std::unique_ptr<ExceptionTableEntry>>& getExceptionTable() const {
        return _exceptionTable;
    }
};

inline void CodeAttribute::readInfo(ClassReader* reader) {
    _maxStack = reader->read_uint16();
    _maxLocals = reader->read_uint16();
    uint32_t codeLength = reader->read_uint32();
    _code = reader->read_bytes(codeLength);
    _exceptionTable = readExceptionTable(reader);
    _attributes = readAttributes(reader, _cp);
}

inline std::vector<std::unique_ptr<ExceptionTableEntry>> 
CodeAttribute::readExceptionTable(ClassReader* reader) {
    uint16_t exceptionTableLength = reader->read_uint16();
    std::vector<std::unique_ptr<ExceptionTableEntry>> exceptionTable;
    exceptionTable.reserve(exceptionTableLength);

    for (uint16_t i = 0; i < exceptionTableLength; i++) {
        uint16_t startPc = reader->read_uint16();
        uint16_t endPc = reader->read_uint16();
        uint16_t handlerPc = reader->read_uint16();
        uint16_t catchType = reader->read_uint16();
        
        exceptionTable.push_back(std::make_unique<ExceptionTableEntry>(
            startPc, endPc, handlerPc, catchType));
    }

    return exceptionTable;
}

///// ExceptionsAttribute 用于指定异常类型
class ExceptionsAttribute : public AttributeInfo {
private:
    // uint16_t _numberOfExceptions;
    std::vector<uint16_t> _exceptionIndexTable;

public:
    void readInfo(ClassReader* reader) override {
        _exceptionIndexTable = reader->read_uint16s();
    }

    const std::vector<uint16_t>& getExceptionIndexTable() const {
        return _exceptionIndexTable;
    }
};

///// LineNumberTableAttribute 和 LocalVariableTableAttribute 用于调试信息
class LineNumberTableEntry {
private:
    uint16_t _startPc;
    uint16_t _lineNumber;

public:
    LineNumberTableEntry(uint16_t startPc, uint16_t lineNumber)
        : _startPc(startPc), _lineNumber(lineNumber) {}

    uint16_t getStartPc() const { return _startPc; }
    uint16_t getLineNumber() const { return _lineNumber; }
};

// LineNumberTableAttribute contains a list of line number information
class LineNumberTableAttribute : public AttributeInfo {
private:
    std::vector<std::unique_ptr<LineNumberTableEntry>> _lineNumberTable;

public:
    void readInfo(ClassReader* reader) override {
        uint16_t lineNumberTableLength = reader->read_uint16();
        _lineNumberTable.reserve(lineNumberTableLength);

        for (uint16_t i = 0; i < lineNumberTableLength; i++) {
            uint16_t startPc = reader->read_uint16();
            uint16_t lineNumber = reader->read_uint16();
            _lineNumberTable.push_back(
                std::make_unique<LineNumberTableEntry>(startPc, lineNumber));
        }
    }

    int getLineNumber(int pc) const {
        // Search from end to beginning to find the most precise line number
        for (auto it = _lineNumberTable.rbegin(); it != _lineNumberTable.rend(); ++it) {
            if (pc >= (*it)->getStartPc()) {
                return (*it)->getLineNumber();
            }
        }
        return -1;
    }
};

class LocalVariableTableEntry {
private:
    uint16_t _startPc;
    uint16_t _length;
    uint16_t _nameIndex;
    uint16_t _descriptorIndex;
    uint16_t _index;

public:
    LocalVariableTableEntry(uint16_t startPc, uint16_t length, 
                            uint16_t nameIndex, uint16_t descriptorIndex, 
                            uint16_t index)
        : _startPc(startPc)
        , _length(length)
        , _nameIndex(nameIndex)
        , _descriptorIndex(descriptorIndex)
        , _index(index) {}

    uint16_t getStartPc() const { return _startPc; }
    uint16_t getLength() const { return _length; }
    uint16_t getNameIndex() const { return _nameIndex; }
    uint16_t getDescriptorIndex() const { return _descriptorIndex; }
    uint16_t getIndex() const { return _index; }
};

class LocalVariableTableAttribute : public AttributeInfo {
private:
    std::vector<std::unique_ptr<LocalVariableTableEntry>> _localVariableTable;

public:
    void readInfo(ClassReader* reader) override {
        uint16_t localVariableTableLength = reader->read_uint16();
        _localVariableTable.reserve(localVariableTableLength);

        for (uint16_t i = 0; i < localVariableTableLength; i++) {
            uint16_t startPc = reader->read_uint16();
            uint16_t length = reader->read_uint16();
            uint16_t nameIndex = reader->read_uint16();
            uint16_t descriptorIndex = reader->read_uint16();
            uint16_t index = reader->read_uint16();

            _localVariableTable.push_back(
                std::make_unique<LocalVariableTableEntry>(
                    startPc, length, nameIndex, descriptorIndex, index));
        }
    }

    const std::vector<std::unique_ptr<LocalVariableTableEntry>>& 
    getLocalVariableTable() const {
        return _localVariableTable;
    }
};

/////////////////// Attribute classes ///////////////////////


inline std::vector<std::unique_ptr<AttributeInfo>> readAttributes(ClassReader* reader, const ConstantPool& cp) {
    uint16_t attributesCount = reader->read_uint16();
    std::vector<std::unique_ptr<AttributeInfo>> attributes;
    attributes.reserve(attributesCount);
    LOG(INFO, "attributes count: %d", attributesCount);
    
    for (uint16_t i = 0; i < attributesCount; i++) {
        attributes.push_back(readAttribute(reader, cp));
    }
    return attributes;
}

inline std::unique_ptr<AttributeInfo> readAttribute(ClassReader* reader, const ConstantPool& cp) {
    uint16_t attrNameIndex = reader->read_uint16();
    std::string attrName = cp.get_utf8(attrNameIndex);
    uint32_t attrLen = reader->read_uint32();
    LOG(INFO, "attr_index: %d attribute name: %s, length: %d", attrNameIndex, attrName.c_str(), attrLen);
    
    auto attrInfo = newAttributeInfo(attrName, attrLen, cp);
    attrInfo->readInfo(reader);
    return attrInfo;
}

inline std::unique_ptr<AttributeInfo> newAttributeInfo(const std::string& attrName, 
                                                     uint32_t attrLen, 
                                                     const ConstantPool& cp) {
    if (attrName == "Code") {
        return std::make_unique<CodeAttribute>(cp);
    } else if (attrName == "ConstantValue") {
        return std::make_unique<ConstantValueAttribute>();
    } else if (attrName == "Deprecated") {
        return std::make_unique<DeprecatedAttribute>();
    } else if (attrName == "Exceptions") {
        return std::make_unique<ExceptionsAttribute>();
    } else if (attrName == "LineNumberTable") {
        return std::make_unique<LineNumberTableAttribute>();
    } else if (attrName == "LocalVariableTable") {
        return std::make_unique<LocalVariableTableAttribute>();
    } else if (attrName == "SourceFile") {
        return std::make_unique<SourceFileAttribute>(cp);
    } else if (attrName == "Synthetic") {
        return std::make_unique<SyntheticAttribute>();
    } else {
        return std::make_unique<UnparsedAttribute>(attrName, attrLen);
    }
}


} // namespace classfile 
} // namespace jvm
