#pragma once

#include <vector>
#include <cstdint>
#include "slot.hpp"

namespace jvm {
namespace rtda {

class LocalVars {
private:
    std::vector<Slot> _slots;

public:
    explicit LocalVars(uint32_t maxLocals);
    
    // Basic setters and getters
    void setInt(uint32_t index, int32_t val);
    int32_t getInt(uint32_t index) const;
    
    void setFloat(uint32_t index, float val);
    float getFloat(uint32_t index) const;
    
    void setLong(uint32_t index, int64_t val);
    int64_t getLong(uint32_t index) const;
    
    void setDouble(uint32_t index, double val);
    double getDouble(uint32_t index) const;
    
    void setRef(uint32_t index, Object* pRef);
    Object* getRef(uint32_t index) const;
};

} // namespace rtda
} // namespace jvm