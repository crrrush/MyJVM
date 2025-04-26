
// #include "instructions/base/instructions.h"
#include "instructions.h"

namespace jvm {
namespace instructions {


// NoOperandsInstruction
void NoOperandsInstruction::fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override {
    // No operands to fetch
}

void NoOperandsInstruction::execute(std::shared_ptr<Frame> p_frame) override {
    // No operation
}


// BranchInstruction
void BranchInstruction::fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override {
    _offset = p_rd->readInt16();
}

void BranchInstruction::execute(std::shared_ptr<Frame> p_frame) override {
    // Branch logic to be implemented in derived classes
}

int16_t BranchInstruction::getOffset() const {
    return _offset;
}


// Index8Instruction
void Index8Instruction::fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override {
    _index = p_rd->readUint8();
}

void Index8Instruction::execute(std::shared_ptr<Frame> p_frame) override {
    // Execution logic to be implemented in derived classes
}

uint8_t Index8Instruction::getIndex() const {
    return _index;
}

// Index16Instruction
void Index16Instruction::fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override {
    _index = p_rd->readUint16();
}

void Index16Instruction::execute(std::shared_ptr<Frame> p_frame) override {
    // Execution logic to be implemented in derived classes
}

uint16_t Index16Instruction::getIndex() const {
    return _index;
}


} // namespace instructions
} // namespace jvm

