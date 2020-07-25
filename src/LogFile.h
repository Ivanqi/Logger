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
        const std::string basename_;
        const int flushInterval_;
        const off_t rollSize_;
        const int checkEveryN_;

        int count_;
        std::unique_ptr<MutexLock> mutex_;
        std::unique_ptr<AppendFile> file_;

        time_t startOfPeriod_;
        time_t lastRoll_;
        time_t lastFlush_

        const static int kRollPerSeconds_ = 60 * 60 * 24;

    private:
        void append_unlocked(const char* logfile, int lne);

        static std::string getLogFileName(const std::string& basename, time_t* now);

    public:
        // 每被append，checkEveryN_次。 flush一下，会往文件写。文件也带有缓冲区
        LogFile(const std::string& basename, int checkEveryN_ = 1024);

        ~LogFile();

        // 写文件 append
        void append(const char* logline, int len);
        
        void flush();

        void rollFile();
};

#endif