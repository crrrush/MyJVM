<<<<<<< HEAD
#include "local_vars.h"
// #include <bit>
// #include <cstdint>

namespace jvm {
namespace rtda {

LocalVars::LocalVars(uint32_t maxLocals) {
    if (maxLocals > 0) {
        _slots.resize(maxLocals);
    }
}

void LocalVars::setInt(uint32_t index, int32_t val) {
    _slots[index].setNum(val);
}

int32_t LocalVars::getInt(uint32_t index) const {
    return _slots[index].getNum();
}

void LocalVars::setFloat(uint32_t index, float val) {
    union {
        float f;
        int32_t i;
    } converter;
    converter.f = val;
    _slots[index].setNum(converter.i);
}

float LocalVars::getFloat(uint32_t index) const {
    union {
        int32_t i;
        float f;
    } converter;
    converter.i = _slots[index].getNum();
    return converter.f;
}

void LocalVars::setLong(uint32_t index, int64_t val) {
    // 低32位存储在index位置，高32位存储在index+1位置
    _slots[index].setNum(static_cast<int32_t>(val));
    _slots[index + 1].setNum(static_cast<int32_t>(val >> 32));
}

int64_t LocalVars::getLong(uint32_t index) const {
    // 低32位在index位置，高32位在index+1位置
    int64_t low = static_cast<uint32_t>(_slots[index].getNum());
    int64_t high = static_cast<uint32_t>(_slots[index + 1].getNum());
    return (high << 32) | low;
}

void LocalVars::setDouble(uint32_t index, double val) {
    union {
        double d;
        int64_t i;
    } converter;
    converter.d = val;
    setLong(index, converter.i);
}

double LocalVars::getDouble(uint32_t index) const {
    union {
        int64_t i;
        double d;
    } converter;
    converter.i = getLong(index);
    return converter.d;
}

void LocalVars::setRef(uint32_t index, Object* pRef) {
    _slots[index].setRef(pRef);
}

Object* LocalVars::getRef(uint32_t index) const {
    return _slots[index].getRef();
}

} // namespace rtda
=======
#include "local_vars.h"
// #include <bit>
// #include <cstdint>

namespace jvm {
namespace rtda {

LocalVars::LocalVars(uint32_t maxLocals) {
    if (maxLocals > 0) {
        _slots.resize(maxLocals);
    }
}

void LocalVars::setInt(uint32_t index, int32_t val) {
    _slots[index].setNum(val);
}

int32_t LocalVars::getInt(uint32_t index) const {
    return _slots[index].getNum();
}

void LocalVars::setFloat(uint32_t index, float val) {
    union {
        float f;
        int32_t i;
    } converter;
    converter.f = val;
    _slots[index].setNum(converter.i);
}

float LocalVars::getFloat(uint32_t index) const {
    union {
        int32_t i;
        float f;
    } converter;
    converter.i = _slots[index].getNum();
    return converter.f;
}

void LocalVars::setLong(uint32_t index, int64_t val) {
    // 低32位存储在index位置，高32位存储在index+1位置
    _slots[index].setNum(static_cast<int32_t>(val));
    _slots[index + 1].setNum(static_cast<int32_t>(val >> 32));
}

int64_t LocalVars::getLong(uint32_t index) const {
    // 低32位在index位置，高32位在index+1位置
    int64_t low = static_cast<uint32_t>(_slots[index].getNum());
    int64_t high = static_cast<uint32_t>(_slots[index + 1].getNum());
    return (high << 32) | low;
}

void LocalVars::setDouble(uint32_t index, double val) {
    union {
        double d;
        int64_t i;
    } converter;
    converter.d = val;
    setLong(index, converter.i);
}

double LocalVars::getDouble(uint32_t index) const {
    union {
        int64_t i;
        double d;
    } converter;
    converter.i = getLong(index);
    return converter.d;
}

void LocalVars::setRef(uint32_t index, Object* pRef) {
    _slots[index].setRef(pRef);
}

Object* LocalVars::getRef(uint32_t index) const {
    return _slots[index].getRef();
}

} // namespace rtda
>>>>>>> a8025d7fab866028dca65f3bf553e900af845786
} // namespace jvm