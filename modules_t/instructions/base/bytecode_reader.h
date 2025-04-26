#pragma once

#include "../../log.hpp"
#include <vector>

namespace jvm {
namespace instructions {

class BytecodeReader{
public:
    BytecodeReader();
    BytecodeReader(const std::vector<uint8_t>& code);

    void reset(const std::vector<uint8_t>& code, size_t pc);

    uint8_t readUint8();
    int8_t readInt8();

    uint16_t readUint16();
    int16_t readInt16();

    int32_t readInt32();

    // used by lookupswitch and tableswitch
    std::vector<int32_t> readInt32s(int32_t n);
    // used by lookupswitch and tableswitch
    void skipPadding();
    
private:
    std::vector<uint8_t> _code;
    size_t _pc; // program counter
};


} // namespace instructions
} // namespace jvm
