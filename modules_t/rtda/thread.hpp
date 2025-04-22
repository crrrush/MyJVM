#pragma once

#include <memory>
#include <stdexcept>
#include "stack.hpp" // 引入Stack类

namespace jvm {
namespace rtda {

class Thread{
public:
    Thread(const size_t& st_size = 1024) : _pc(0), _p_stack(std::make_shared<Stack>(st_size)) {}
    Thread(const Thread&) = delete; // 禁止拷贝构造
    Thread& operator=(const Thread&) = delete; // 禁止赋值操作
    Thread(Thread&&) = default; // 允许移动构造
    Thread& operator=(Thread&&) = default; // 允许移动赋值
    ~Thread() = default;

    int getPC() const { return _pc; }
    void setPC(int pc) { _pc = pc; }

    std::shared_ptr<Stack> getStack() const { return _p_stack.get(); }

    void pushFrame(std::shared_ptr<Frame> frame) { _p_stack->push(frame); }
    std::shared_ptr<Frame> popFrame() { return _p_stack->pop(); }

    std::shared_ptr<Frame> currentFrame() const { return _p_stack->top(); }

    // bool isAlive() const { return true; } // Placeholder for actual implementation

private:
    int _pc;
    std::shared_ptr<Stack> _p_stack;
};

} // namespace rtda
} // namespace jvm