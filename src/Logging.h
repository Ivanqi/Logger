#ifndef LOGGER_LOGGING_H
#define LOGGER_LOGGING_H

#include <pthread.h>
#include <string.h>
#include <string>
#include <stdio.h>

#include "LogStream.h"

class Logger
{
    private:
        class RecordBlock
        {
            public:
                RecordBlock(const char *fileName, int line);
                void formatTime();

                LogStream stream_;
                int line_;
                std::string basename_;
        };

        RecordBlock Redcord;
        static std::string logFileName_;
        
    public:
        Logger(const char *fileName, int line);
        ~Logger();
        LogStream& stream()
        {
            return Redcord.stream_;
        }

        static void setLogFileName(std::string fileName)
        {
            logFileName_ = fileName;
        }

        static std::string getLogFileName()
        {
            return logFileName_;
        }

        const char* getBuffer()
        {
            return Redcord.stream_.buffer().data();
        }
};

// 日志打印宏
#define LOG Logger(__FILE__, __LINE__).stream()

#endif