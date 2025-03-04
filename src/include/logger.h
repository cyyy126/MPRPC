#pragma once
#include "lockqueue.h"

//定义宏
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0)  \

#define LOG_ERR(logmsgformat, ...)  \
do \
{  \
    Logger &logger = Logger::GetInstance(); \
    logger.SetLogLevel(ERROR); \
    char c[1024] = {0}; \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c); \
} while(0)  \

enum LogLevel
{
    INFO, // 普通信息
    ERROR // 错误信息
};

//Mprpc框架提供的日志系统
class Logger
{
public:
    // 单例模式
    static Logger& GetInstance();
    // 设置日志级别
    void SetLogLevel(int level);
    // 写日志
    void Log(std::string msg);
private:
    int m_loggerlevel; // 记录日志级别
    LockQueue<std::string> m_lckque; // 日志缓冲队列

    // 写成单例模式
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&) = delete;
};



