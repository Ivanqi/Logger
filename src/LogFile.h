#ifndef LOGGER_LOGFILE_H
#define LOGGER_LOGFILE_H

#include <memory>
#include <string>
#include <mutex>
#include <boost/noncopyable.hpp>

#include "FileUtil.h"
#include "MutexLock.h"

class LogFile: boost::noncopyable
{
    private:
        void append_unlocked(const char* logfile, int lne);

        const std::string basename_;
        const int flushEveryN_;

        int count_;
        std::unique_ptr<MutexLock> mutex_;
        std::unique_ptr<AppendFile> file_;

    public:
        // 每被append，flushEveryN次。 flush一下，会往文件写。文件也带有缓冲区
        LogFile(const std::string& basename, int flushEveryN = 1024);

        ~LogFile();

        // 写文件 append
        void append(const char* logline, int len);
        
        void flush();

        void rollFile();
};

#endif