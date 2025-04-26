<<<<<<< HEAD
#pragma once

// #include <memory>
// #include <stdexcept>
#include <cstdint>

#include "object.hpp" // 引入Object类

namespace jvm {
namespace rtda {

class Object;

class Slot {
private:
    int32_t _num;
    // Object* _p_ref = nullptr;  // 使用nullptr初始化，表明这是一个非拥有型指针
    Object* _p_ref;

public:
    Slot() : _num(0), _p_ref(nullptr) {} // 默认构造函数
    
    // 删除拷贝构造和赋值操作
    Slot(const Slot&) = delete;
    Slot& operator=(const Slot&) = delete;

    // 允许移动构造和赋值
    Slot(Slot&& other) noexcept : _num(other._num), _p_ref(other._p_ref) {
        other._p_ref = nullptr; // 移动后，原对象的引用置为nullptr
    }
    Slot& operator=(Slot&& other) noexcept {
        if (this != &other) {
            _num = other._num;
            _p_ref = other._p_ref;
            other._p_ref = nullptr; // 移动后，原对象的引用置为nullptr
        }
        return *this;
    }
    ~Slot() = default; // 默认析构函数
    
    // getter/setter保持不变
    int32_t getNum() const { return _num; }
    void setNum(int32_t num) { _num = num; }
    Object* getRef() const { return _p_ref; }
    void setRef(Object* ref) { _p_ref = ref; }
};

} // namespace rtda
} // namespace jvm

=======
#pragma once

// #include <memory>
// #include <stdexcept>
#include <cstdint>

#include "object.hpp" // 引入Object类

namespace jvm {
namespace rtda {

class Object;

class Slot {
private:
    int32_t _num;
    // Object* _p_ref = nullptr;  // 使用nullptr初始化，表明这是一个非拥有型指针
    Object* _p_ref;

public:
    Slot() : _num(0), _p_ref(nullptr) {} // 默认构造函数
    
    // 删除拷贝构造和赋值操作
    Slot(const Slot&) = delete;
    Slot& operator=(const Slot&) = delete;

    // 允许移动构造和赋值
    Slot(Slot&& other) noexcept : _num(other._num), _p_ref(other._p_ref) {
        other._p_ref = nullptr; // 移动后，原对象的引用置为nullptr
    }
    Slot& operator=(Slot&& other) noexcept {
        if (this != &other) {
            _num = other._num;
            _p_ref = other._p_ref;
            other._p_ref = nullptr; // 移动后，原对象的引用置为nullptr
        }
        return *this;
    }
    ~Slot() = default; // 默认析构函数
    
    // getter/setter保持不变
    int32_t getNum() const { return _num; }
    void setNum(int32_t num) { _num = num; }
    Object* getRef() const { return _p_ref; }
    void setRef(Object* ref) { _p_ref = ref; }
};

} // namespace rtda
} // namespace jvm

>>>>>>> a8025d7fab866028dca65f3bf553e900af845786
