#include <assert.h>
#include <iostream>
#include <time.h>  
#include <sys/time.h> 
#include "Logging.h"


Logger::RecordBlock::RecordBlock(const char *fileName, int line)
    : stream_(), line_(line), basename_(fileName)
{
    formatTime();
}

// 记录当前时间
void Logger::RecordBlock::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};

    gettimeofday(&tv, NULL);
    time = tv.tv_sec;

    struct tm* p_time = localtime(&time);

    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char *fileName, int line): Redcord(fileName, line) {}
Logger::~Logger() {
    Redcord.stream_ << " -- " << Redcord.basename_ << ':' << Redcord.line_ << '\n';
}