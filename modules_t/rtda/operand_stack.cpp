
#include "operand_stack.h"
#include "../log.hpp"

namespace jvm {
namespace rtda {

void OperandStack::pushInt(const int32_t& value)
{
    if (_top >= _slots.size()) {
        LOG(ERROR, "OperandStack overflow");
        throw std::out_of_range("OperandStack overflow");
    }
    _slots[_top].setNum(value);
    ++_top;
}

int32_t OperandStack::popInt()
{
    if (_top == 0) {
        LOG(ERROR, "OperandStack underflow");
        throw std::out_of_range("OperandStack underflow");
    }
    --_top;
    return _slots[_top].getNum();
}

void OperandStack::pushFloat(const float& value)
{
    if (_top >= _slots.size()) {
        LOG(ERROR, "OperandStack overflow");
        throw std::out_of_range("OperandStack overflow");
    }
    union {
        float f;
        int32_t i;
    } converter;
    converter.f = value;
    _slots[_top].setNum(converter.i);
    ++_top;
}

float OperandStack::popFloat()
{
    if (_top == 0) {
        LOG(ERROR, "OperandStack underflow");
        throw std::out_of_range("OperandStack underflow");
    }
    --_top;
    union {
        int32_t i;
        float f;
    } converter;
    converter.i = _slots[_top].getNum();
    return converter.f;
}

void OperandStack::pushLong(const int64_t& value)
{
    if (_top + 1 >= _slots.size()) {
        LOG(ERROR, "OperandStack overflow");
        throw std::out_of_range("OperandStack overflow");
    }
    // 高32位在顶部，低32位在下面
    _slots[_top].setNum(static_cast<int32_t>(value & 0xFFFFFFFF));
    ++_top;
    _slots[_top].setNum(static_cast<int32_t>((value >> 32) & 0xFFFFFFFF));
    ++_top;
}

int64_t OperandStack::popLong()
{
    if (_top < 2) {
        LOG(ERROR, "OperandStack underflow");
        throw std::out_of_range("OperandStack underflow");
    }
    --_top;
    int64_t high = static_cast<int64_t>(_slots[_top].getNum()) & 0xFFFFFFFF;
    --_top;
    int64_t low = static_cast<int64_t>(_slots[_top].getNum()) & 0xFFFFFFFF;
    return (high << 32) | low;
}

void OperandStack::pushDouble(const double& value)
{
    if (_top + 1 >= _slots.size()) {
        LOG(ERROR, "OperandStack overflow");
        throw std::out_of_range("OperandStack overflow");
    }
    union {
        double d;
        long l;
    } converter;
    converter.d = value;
    pushLong(converter.l);
}

double OperandStack::popDouble()
{
    if (_top < 2) {
        LOG(ERROR, "OperandStack underflow");
        throw std::out_of_range("OperandStack underflow");
    }
    union {
        long l;
        double d;
    } converter;
    converter.l = popLong();
    return converter.d;
}

void OperandStack::pushRef(Object* ref)
{
    if (_top >= _slots.size()) {
        LOG(ERROR, "OperandStack overflow");
        throw std::out_of_range("OperandStack overflow");
    }
    _slots[_top].setRef(ref);
    ++_top;
}

Object* OperandStack::popRef()
{
    if (_top == 0) {
        LOG(ERROR, "OperandStack underflow");
        throw std::out_of_range("OperandStack underflow");
    }
    --_top;
    return _slots[_top].getRef();
}

} // namespace rtda
} // namespace jvms
    
