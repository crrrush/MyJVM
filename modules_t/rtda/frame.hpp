<<<<<<< HEAD
#pragma once

#include <memory>
#include <stdexcept>

#include "local_vars.h"
#include "operand_stack.h"

namespace jvm {
namespace rtda {

class Frame{
public:
    Frame(const size_t& max_lac_size, const size_t& max_op_size) 
        : _local_vars(max_lac_size), 
          _p_operand_stack(std::make_shared<OperandStack>(max_op_size)) {};

    Frame(const Frame&) = delete; // 禁止拷贝构造
    Frame& operator=(const Frame&) = delete; // 禁止赋值操作
    Frame(Frame&&) = default; // 允许移动构造
    Frame& operator=(Frame&&) = default; // 允许移动赋值
    ~Frame() = default;

    LocalVars& getLoaclVars() {
        return _local_vars;
    }

    std::shared_ptr<OperandStack> getOperandStack() {
        return _p_operand_stack;
    }

private:
    LocalVars _local_vars;
    std::shared_ptr<OperandStack> _p_operand_stack;
};

} // namespace rtda
} // namespace jvm
=======
#pragma once

#include <memory>
#include <stdexcept>

#include "local_vars.h"
#include "operand_stack.h"

namespace jvm {
namespace rtda {

class Frame{
public:
    Frame(const size_t& max_lac_size, const size_t& max_op_size) 
        : _local_vars(max_lac_size), 
          _p_operand_stack(std::make_shared<OperandStack>(max_op_size)) {};

    Frame(const Frame&) = delete; // 禁止拷贝构造
    Frame& operator=(const Frame&) = delete; // 禁止赋值操作
    Frame(Frame&&) = default; // 允许移动构造
    Frame& operator=(Frame&&) = default; // 允许移动赋值
    ~Frame() = default;

    LocalVars& getLoaclVars() {
        return _local_vars;
    }

    std::shared_ptr<OperandStack> getOperandStack() {
        return _p_operand_stack;
    }

private:
    LocalVars _local_vars;
    std::shared_ptr<OperandStack> _p_operand_stack;
};

} // namespace rtda
} // namespace jvm
>>>>>>> a8025d7fab866028dca65f3bf553e900af845786
