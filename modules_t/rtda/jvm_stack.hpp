#pragma once


#include <list>
#include <memory>
#include <stdexcept>

#include "frame.hpp" // 引入Frame类

namespace jvm {
namespace rtda {

class JvmStack {
public:
    JvmStack(size_t max_size) : _max_size(max_size), _size(0), _top(nullptr) {}
    JvmStack(const JvmStack&) = delete; // 禁止拷贝构造
    JvmStack& operator=(const JvmStack&) = delete; // 禁止赋值操作
    JvmStack(JvmStack&&) = default; // 允许移动构造
    JvmStack& operator=(JvmStack&&) = default; // 允许移动赋值
    ~JvmStack() = default;

    void push(std::shared_ptr<Frame> frame) {
        if (_size >= _max_size) {
            throw std::runtime_error("java.lang.StackOverflowError");
        }
        _frames.push_back(frame);
        _top = frame;
        ++_size;
    }

    std::shared_ptr<Frame> pop() {
        if (_size == 0) {
            throw std::runtime_error("Jvm Stack underflow");
        }
        auto frame = _top;
        _frames.pop_back();
        if (_frames.empty()) {
            _top = nullptr;
        } else {
            _top = _frames.back();
        }
        --_size;
        return frame;
    }

    std::shared_ptr<Frame> top() const 
    {
        if (_top == nullptr) {
            throw std::runtime_error("Jvm Stack is empty");
        } 
        return _top; 
    }

private:
    size_t _max_size;
    size_t _size;
    std::shared_ptr<Frame> _top;
    std::list<std::shared_ptr<Frame>> _frames;
};

} // namespace rtda
} // namespace jvm
