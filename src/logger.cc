#include "logger.h"
#include "time.h"
#include <iostream>

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        for(;;)
        {
            // 获取当前日期，然后取日志信息，把相应的信息写入到日志文件当中，a+
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if (pf == nullptr)
            {
                std::cout << "logger file: " << file_name << "open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckque.pop();
            char time_buf[1024];
            sprintf(time_buf, "%d:%d:%d => ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), pf);
            fclose(pf);
        }

    });
    writeLogTask.detach();
}

// 设置日志级别
void Logger::SetLogLevel(int level)
{
    m_loggerlevel = level;
}
// 写日志
void Logger::Log(std::string msg)
{
    m_lckque.push(msg);
}