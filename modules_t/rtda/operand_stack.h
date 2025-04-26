<<<<<<< HEAD
#pragma once

#include <stdexcept>
#include <vector>

#include "slot.hpp"

namespace jvm {
namespace rtda {

class OperandStack {
public:
    explicit OperandStack(size_t size) : _top(0), _slots(size) {}

    OperandStack(const OperandStack&) = delete; // 禁止拷贝构造
    OperandStack& operator=(const OperandStack&) = delete; // 禁止拷贝赋值

    OperandStack(OperandStack&&) = default;
    OperandStack& operator=(OperandStack&&) = default;

    void pushInt(const int32_t& value);
    int32_t popInt();

    void pushFloat(const float& value);
    float popFloat();

    void pushLong(const int64_t& value);
    int64_t popLong();

    void pushDouble(const double& value);
    double popDouble();

    void pushRef(Object* ref);
    Object* popRef();


private:
    size_t _top;
    std::vector<Slot> _slots;
};

} // namespace rtda
} // namespace jvms
=======
#pragma once

#include <stdexcept>
#include <vector>

#include "slot.hpp"

namespace jvm {
namespace rtda {

class OperandStack {
public:
    explicit OperandStack(size_t size) : _top(0), _slots(size) {}

    OperandStack(const OperandStack&) = delete; // 禁止拷贝构造
    OperandStack& operator=(const OperandStack&) = delete; // 禁止拷贝赋值

    OperandStack(OperandStack&&) = default;
    OperandStack& operator=(OperandStack&&) = default;

    void pushInt(const int32_t& value);
    int32_t popInt();

    void pushFloat(const float& value);
    float popFloat();

    void pushLong(const int64_t& value);
    int64_t popLong();

    void pushDouble(const double& value);
    double popDouble();

    void pushRef(Object* ref);
    Object* popRef();


private:
    size_t _top;
    std::vector<Slot> _slots;
};

} // namespace rtda
} // namespace jvms
>>>>>>> a8025d7fab866028dca65f3bf553e900af845786
