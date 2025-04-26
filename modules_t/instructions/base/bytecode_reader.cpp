
// #include "base/bytecode_reader.h"
#include "bytecode_reader.h"

namespace jvm {
namespace instructions {


BytecodeReader::BytecodeReader() : _pc(0) {}
BytecodeReader::BytecodeReader(const std::vector<uint8_t>& code) : _code(code), _pc(0) {}

void BytecodeReader::reset(const std::vector<uint8_t>& code, size_t pc) {
    _code = code;
    _pc = pc;
}

uint8_t BytecodeReader::readUint8() {
    if (_pc >= _code.size()) {
        LOG(ERROR, "BytecodeReader: Out of range");
        throw std::out_of_range("BytecodeReader: Out of range");
    }
    return _code[_pc++];
}

int8_t BytecodeReader::readInt8() {
    if (_pc >= _code.size()) {
        LOG(ERROR, "BytecodeReader: Out of range");
        throw std::out_of_range("BytecodeReader: Out of range");
    }
    return static_cast<int8_t>(_code[_pc++]);
}

uint16_t BytecodeReader::readUint16() {
    if (_pc + 1 >= _code.size()) {
        LOG(ERROR, "BytecodeReader: Out of range");
        throw std::out_of_range("BytecodeReader: Out of range");
    }
    uint16_t value = readUint8();
    value = (value << 8) | readUint8();
    return value;
}

int16_t BytecodeReader::readInt16() {
    if (_pc + 1 >= _code.size()) {
        LOG(ERROR, "BytecodeReader: Out of range");
        throw std::out_of_range("BytecodeReader: Out of range");
    }
    return static_cast<int16_t>(readUint16());
}

int32_t BytecodeReader::readInt32() {
    if (_pc + 3 >= _code.size()) {
        LOG(ERROR, "BytecodeReader: Out of range");
        throw std::out_of_range("BytecodeReader: Out of range");
    }
    int32_t value = readUint8();
    value = (value << 8) | readUint8();
    value = (value << 8) | readUint8();
    value = (value << 8) | readUint8();
    return value;
}

// used by lookupswitch and tableswitch
std::vector<int32_t> BytecodeReader::readInt32s(int32_t n) {
    if (_pc + 3 >= _code.size()) {
        LOG(ERROR, "BytecodeReader: Out of range");
        throw std::out_of_range("BytecodeReader: Out of range");
    }
    std::vector<int32_t> ints(n);
    for(auto& e : ints) {
        e = readInt32();
    }
    return ints;
}

// used by lookupswitch and tableswitch
void BytecodeReader::skipPadding() {
    while (_pc % 4 != 0) {
        readUint8();
    }
}

} // namespace instructions
} // namespace jvm
