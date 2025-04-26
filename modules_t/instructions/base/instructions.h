#pragma once

#include <memory>
#include <cstdint>

#include "bytecode_reader.h"
#include "../../rtda/frame.hpp"

namespace jvm {
namespace instructions {

class Instruction{
public:
    virtual ~Instruction() = default;

    // 从字节码中提取操作数
    virtual void fetchOperands(std::shared_ptr<BytecodeReader> p_rd) = 0;

    // 执行指令逻辑
    virtual void execute(std::shared_ptr<Frame> p_frame) = 0;

    // 获取指令名称
    // virtual const char* name() const = 0;
};

class NoOperandsInstruction : public Instruction {
public:
    void fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override;
    void execute(std::shared_ptr<Frame> p_frame) override;
};

class BranchInstruction : public Instruction {
public:
    void fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override;
    void execute(std::shared_ptr<Frame> p_frame) override;

    int16_t getOffset() const;

private:
    int16_t _offset;
};

class Index8Instruction : public Instruction {
public:
    void fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override;

    void execute(std::shared_ptr<Frame> p_frame) override;

    uint8_t getIndex() const;

private:
    uint8_t _index;
};

class Index16Instruction : public Instruction {
public:
    void fetchOperands(std::shared_ptr<BytecodeReader> p_rd) override;

    void execute(std::shared_ptr<Frame> p_frame) override;

    uint16_t getIndex() const;
private:
    uint16_t _index;
};


} // namespace instructions
} // namespace jvm

