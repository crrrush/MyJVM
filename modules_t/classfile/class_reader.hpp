#pragma once

#include <vector>
// #include "../log.hpp"  // 自定义日志模块（需确保项目中有该头文件）
#include "../util.hpp"  // 工具类（需确保项目中有该头文件）

namespace jvm {
namespace classfile {

class ClassReader 
{
public:
    // 构造函数，接受字节码数据
    explicit ClassReader(const std::vector<uint8_t>& data) 
        : _data(data), _offset(0) {}

    // 读取单字节 (u1)
    uint8_t read_uint8() 
    {
        if (_offset >= _data.size()) 
        {
            throw std::out_of_range("ClassReader: Read out of range");
        }
        return _data[_offset++]; 
    }
    
    // 读取两个字节 (u2)，需要处理大端字节序
    uint16_t read_uint16() 
    {
        if( _offset + 1 >= _data.size()) 
        {
            throw std::out_of_range("ClassReader: Read out of range");
        }
        uint16_t val = util::util_byte_order::bigToHost16(&_data[_offset]);
        _offset += 2;
        return val;
    }

    // 读取四个字节 (u4)，需要处理大端字节序
    uint32_t read_uint32()
    {
        if( _offset + 3 >= _data.size()) 
        {
            throw std::out_of_range("ClassReader: Read out of range");
        }
        uint32_t val = util::util_byte_order::bigToHost32(&_data[_offset]);
        _offset += 4;
        return val;
    }
    
    // 读取八个字节 (u8)，需要处理大端字节序
    uint64_t read_uint64() 
    {
        if( _offset + 7 >= _data.size()) 
        {
            throw std::out_of_range("ClassReader: Read out of range");
        }
        uint64_t val = util::util_byte_order::bigToHost64(&_data[_offset]);
        _offset += 8;
        return val;
    }

    // 读取uint16数组
    std::vector<uint16_t> read_uint16s() 
    {
        uint16_t n = read_uint16();
        if( _offset + n * 2 > _data.size()) 
        {
            throw std::out_of_range("ClassReader: Read out of range");
        }
        std::vector<uint16_t> s(n);
        for (uint16_t i = 0; i < n; i++) 
        {
            s[i] = read_uint16();
        }
        return s;
    }

    // 读取指定长度的字节数组
    std::vector<uint8_t> read_bytes(uint32_t n) 
    {
        if( _offset + n > _data.size()) 
        {
            throw std::out_of_range("ClassReader: Read out of range");
        }
        std::vector<uint8_t> bytes(_data.begin() + _offset,  _data.begin() + _offset + n);
        _offset += n;
        return bytes;
    }

private:
    std::vector<uint8_t> _data; // 字节码数据
    size_t _offset;            // 当前读取位置
};


} // namespace classfile
} // namespace jvm
