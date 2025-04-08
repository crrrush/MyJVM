#pragma once  // 头文件保护，防止重复包含

#include <iostream>   // 标准输入输出流
#include <fstream>    // 文件输入输出流
#include <sstream>    // 字符串流处理
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
}
