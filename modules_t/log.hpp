#pragma once

#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <ctime>

// enum
enum LEVEL
{
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
};

#define DEFAULT_LEVEL DEBUG

#define LOGBUFFERSIZE 1024
#define TMBUFSIZE 32

#define LOG(level, format, ...) do{\
    if (level < DEFAULT_LEVEL) break;\
    time_t t = time(NULL);\
    struct tm *lt = localtime(&t);\
    char buf[32] = {0};\
    strftime(buf, 31, "%H:%M:%S", lt);\
    fprintf(stdout, "[%s %s:%d] " format "\n", buf, __FILE__, __LINE__, ##__VA_ARGS__);\
}while(0)
