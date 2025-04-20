#pragma once  // 头文件保护，防止重复包含

#include <iostream>   // 标准输入输出流
#include <fstream>    // 文件输入输出流
#include <sstream>    // 字符串流处理
#include <cstdint>
#include <cstring>   // 字符串处理
#include <algorithm>
#include "log.hpp"    // 自定义日志模块（需确保项目中有该头文件）

namespace util  // 工具类命名空间，封装通用工具函数
{
    /**
     * @brief 文件操作工具类
     * 提供静态文件读写方法，无需实例化即可使用
     */
    class util_file
    {
    public:
        /**
         * @brief 读取整个文件内容到字符串
         * @param filename - 要读取的文件路径
         * @param body - 输出参数，存储文件内容的字符串
         * @return bool - 操作是否成功
         * 
         * 功能流程：
         * 1. 以二进制模式打开文件（避免换行符转换问题）
         * 2. 检查文件是否成功打开
         * 3. 获取文件大小并预分配内存
         * 4. 读取文件内容到字符串
         * 5. 检查读取过程是否出错
         * 6. 确保文件流正确关闭
         */
        static bool read(const std::string& filename, std::string& body)
        {
            // 创建输入文件流对象，以二进制模式打开
            std::ifstream ifs(filename, std::ios::binary);
            
            // 检查文件是否成功打开
            if (!ifs.is_open())
            {
                LOG(ERROR, "%s file open failed!!", filename.c_str());  // 使用日志模块记录错误
                return false;
            }

            // 获取文件大小
            size_t fsize = 0;
            ifs.seekg(0, std::ios::end);    // 将文件指针移动到末尾
            fsize = ifs.tellg();            // 获取当前指针位置（即文件大小）
            ifs.seekg(0, std::ios::beg);    // 将文件指针重置到开头

            // 预分配内存空间（提升读取效率）
            body.resize(fsize);

            // 读取文件内容到字符串
            ifs.read(&body[0], fsize);
            
            // 检查读取过程是否出错
            if (!ifs.good())
            {
                LOG(ERROR, "read %s file content failed!", filename.c_str());
                ifs.close();  // 显式关闭文件流
                return false;
            }

            ifs.close();  // 确保文件流正确关闭
            return true;
        }
    };

    
    // /// @brief 字节序转换工具类，提供大端字节序与主机字节序之间的转换功能。
    // class util_byte_order {
    // public:
    //     /// @brief 将大端字节序的 16 位数据转换为主机字节序。
    //     /// @param bytes 指向大端字节序数据的指针，至少包含 2 个字节。
    //     /// @return 转换为主机字节序后的 16 位无符号整数。
    //     static uint16_t bigToHost16(const uint8_t* bytes) {
    //         uint16_t value = (static_cast<uint16_t>(bytes[0]) << 8) |
    //                          static_cast<uint16_t>(bytes[1]);
    //         if (!isBigEndian()) {
    //             return (value >> 8) | (value << 8);
    //         }
    //         return value;
    //     }
    
    //     /// @brief 将大端字节序的 32 位数据转换为主机字节序。
    //     /// @param bytes 指向大端字节序数据的指针，至少包含 4 个字节。
    //     /// @return 转换为主机字节序后的 32 位无符号整数。
    //     static uint32_t bigToHost32(const uint8_t* bytes) {
    //         uint32_t value = (static_cast<uint32_t>(bytes[0]) << 24) |
    //                          (static_cast<uint32_t>(bytes[1]) << 16) |
    //                          (static_cast<uint32_t>(bytes[2]) << 8)  |
    //                          static_cast<uint32_t>(bytes[3]);
    //         if (!isBigEndian()) {
    //             return ((value >> 24) & 0xFF) |
    //                    ((value >> 8)  & 0xFF00) |
    //                    ((value << 8)  & 0xFF0000) |
    //                    ((value << 24) & 0xFF000000);
    //         }
    //         return value;
    //     }
    
    //     /// @brief 将大端字节序的 64 位数据转换为主机字节序。
    //     /// @param bytes 指向大端字节序数据的指针，至少包含 8 个字节。
    //     /// @return 转换为主机字节序后的 64 位无符号整数。
    //     static uint64_t bigToHost64(const uint8_t* bytes) {
    //         uint64_t value = (static_cast<uint64_t>(bytes[0]) << 56) |
    //                          (static_cast<uint64_t>(bytes[1]) << 48) |
    //                          (static_cast<uint64_t>(bytes[2]) << 40) |
    //                          (static_cast<uint64_t>(bytes[3]) << 32) |
    //                          (static_cast<uint64_t>(bytes[4]) << 24) |
    //                          (static_cast<uint64_t>(bytes[5]) << 16) |
    //                          (static_cast<uint64_t>(bytes[6]) << 8)  |
    //                          static_cast<uint64_t>(bytes[7]);
    //         if (!isBigEndian()) {
    //             return ((value >> 56) & 0xFF) |
    //                    ((value >> 40) & 0xFF00) |
    //                    ((value >> 24) & 0xFF0000) |
    //                    ((value >> 8)  & 0xFF000000) |
    //                    ((value << 8)  & 0xFF00000000) |
    //                    ((value << 24) & 0xFF0000000000) |
    //                    ((value << 40) & 0xFF000000000000) |
    //                    ((value << 56) & 0xFF00000000000000);
    //         }
    //         return value;
    //     }
    
    //     /// @brief 检查当前主机是否使用大端字节序。
    //     /// @return 如果主机是大端字节序，返回 true；否则返回 false。
    //     static bool isBigEndian() {
    //         static const uint16_t test = 0x1234;
    //         static const bool is_big = (*reinterpret_cast<const uint8_t*>(&test) == 0x12);
    //         return is_big;
    //     }
    
    // private:
    //     // 工具类无需实例化，构造函数设为私有
    //     util_byte_order() = delete;
    //     util_byte_order(const util_byte_order&) = delete;
    //     util_byte_order& operator=(const util_byte_order&) = delete;
    // };

    /// @brief 字节序转换工具类，提供大端字节序与主机字节序之间的转换功能。
    class util_byte_order {
    public:
        /// @brief 将大端字节序的 16 位数据转换为主机字节序。
        /// @param bytes 指向大端字节序数据的指针，至少包含 2 个字节。
        /// @return 转换为主机字节序后的 16 位无符号整数。
        static uint16_t bigToHost16(const uint8_t* bytes) {
            // LOG(INFO, "bigToHost16: %02X %02X", bytes[0], bytes[1]);
            return swapIfLittleEndian<uint16_t>(
                (static_cast<uint16_t>(bytes[0])) | (static_cast<uint16_t>(bytes[1]) << 8)
            );
        }

        /// @brief 将大端字节序的 32 位数据转换为主机字节序。
        /// @param bytes 指向大端字节序数据的指针，至少包含 4 个字节。
        /// @return 转换为主机字节序后的 32 位无符号整数。
        static uint32_t bigToHost32(const uint8_t* bytes) {
            return swapIfLittleEndian(
                (static_cast<uint32_t>(bytes[0]))        |
                (static_cast<uint32_t>(bytes[1]) << 8)   |
                (static_cast<uint32_t>(bytes[2]) << 16)  |
                (static_cast<uint32_t>(bytes[3]) << 24)
            );
        }

        /// @brief 将大端字节序的 64 位数据转换为主机字节序。
        /// @param bytes 指向大端字节序数据的指针，至少包含 8 个字节。
        /// @return 转换为主机字节序后的 64 位无符号整数。
        static uint64_t bigToHost64(const uint8_t* bytes) {
            return swapIfLittleEndian(
                (static_cast<uint64_t>(bytes[7]) << 56) |
                (static_cast<uint64_t>(bytes[6]) << 48) |
                (static_cast<uint64_t>(bytes[5]) << 40) |
                (static_cast<uint64_t>(bytes[4]) << 32) |
                (static_cast<uint64_t>(bytes[3]) << 24) |
                (static_cast<uint64_t>(bytes[2]) << 16) |
                (static_cast<uint64_t>(bytes[1]) << 8)  |
                static_cast<uint64_t>(bytes[0])
            );
        }

        /// @brief 检查当前主机是否使用大端字节序。
        /// @return 如果主机是大端字节序，返回 true；否则返回 false。
        static bool isBigEndian() {
            static const uint16_t test = 0x1234;
            static const bool is_big = (*reinterpret_cast<const uint8_t*>(&test) == 0x12);
            return is_big;
        }
    private:
        util_byte_order() = delete;
        util_byte_order(const util_byte_order&) = delete;
        util_byte_order& operator=(const util_byte_order&) = delete;

        template <typename T>
        static T swapIfLittleEndian(T value) {
            if (!isBigEndian()) {
                union {
                    T value;
                    uint8_t bytes[sizeof(T)];
                } u;
                u.value = value;
                std::reverse(u.bytes, u.bytes + sizeof(T));
                return u.value;
            }
            return value;
        }
    };

    // 添加在 namespace util 中，其他工具类的旁边
    /// @brief MUTF-8字符串编码转换工具类
    class util_mutf8 {
    public:
        /**
         * @brief 将Modified UTF-8编码转换为标准UTF-8字符串
         * @param bytes Modified UTF-8编码的字节数组
         * @return 转换后的标准UTF-8字符串
         * 
         * Modified UTF-8与标准UTF-8的主要区别：
         * 1. null字符(0x0000)使用两字节0xC0,0x80表示
         * 2. 补充字符使用6字节表示而不是4字节
         */
        static std::string decode(const std::vector<uint8_t>& bytes) {
            std::string result;
            result.reserve(bytes.size());
            
            for (size_t i = 0; i < bytes.size(); ) {
                uint32_t b1 = bytes[i++];
                
                if ((b1 & 0x80) == 0) {  // 1字节ASCII字符
                    result += static_cast<char>(b1);
                }
                else if ((b1 & 0xE0) == 0xC0) {  // 2字节序列
                    if (i >= bytes.size()) break;  // 防止越界
                    uint32_t b2 = bytes[i++];
                    
                    // 检查是否为null字符的特殊表示
                    if (b1 == 0xC0 && b2 == 0x80) {
                        result += '\0';
                    } else {
                        char ch = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                        result += ch;
                    }
                }
                else if ((b1 & 0xF0) == 0xE0) {  // 3字节序列
                    if (i + 1 >= bytes.size()) break;  // 防止越界
                    uint32_t b2 = bytes[i++];
                    uint32_t b3 = bytes[i++];
                    
                    char32_t codepoint = ((b1 & 0x0F) << 12) | 
                                       ((b2 & 0x3F) << 6)  |
                                       (b3 & 0x3F);
                                       
                    // 将UTF-32编码点转换为UTF-8字符序列
                    if (codepoint < 0x80) {
                        result += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        result += static_cast<char>(0xC0 | (codepoint >> 6));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        result += static_cast<char>(0xE0 | (codepoint >> 12));
                        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                }
            }
            return result;
        }
    
    private:
        // 工具类禁用构造和赋值
        util_mutf8() = delete;
        util_mutf8(const util_mutf8&) = delete;
        util_mutf8& operator=(const util_mutf8&) = delete;
    };
    
}
