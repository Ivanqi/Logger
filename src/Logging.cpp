#include <assert.h>
#include <iostream>
#include <time.h>  
#include <errno.h>
#include <sys/time.h> 
#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include "Timestamp.h"

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;

std::string Logger::logFileName_ = "/web.log";

void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const char* msg, int len)
{
    pthread_once(&once_control_, once_init);
    AsyncLogger_->append(msg, len);
}

__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

Logger::LogLevel initLogLevel()
{
    if (::getenv("LOG_TRACE")) {
        return Logger::TRACE;
    } else if (::getenv("LOG_DEBUG")) {
        return Logger::DEBUG;
    } else {
        return Logger::INFO;
    }
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

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
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}