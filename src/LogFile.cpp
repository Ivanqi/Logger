#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "LogFile.h"
#include "FileUtil.h"

LogFile::LogFile(const std::string& basename, int checkEveryN_)
    :basename_(basename),
    checkEveryN_(checkEveryN_),
    count_(0),
    mutex_(new MutexLock)
{
    file_.reset(new AppendFile(basename));
}

LogFile::~LogFile() {}

void LogFile::append(const char* logline, int len)
{
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush()
{
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) 
{
    file_->append(logline, len);
    if (file_->writtenBytes() > rollSize_) {
        rollFile();
    } else {
        ++count_;
        if (count_ >= checkEveryN_) {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPeriod_ != startOfPeriod_) {
                rollFile();
            } else if (now - lastFlush_ > flushInterval_) {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }
}

bool LogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > lastRoll_) {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new AppendFile(filename));
        return true;
    } else {
        return false;
    }
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;

    *now = time(NULL);
    gmtime_r(now, &tm);

    strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += "unknownhost";

    char pidbuf[32];

    snprintf(pidbuf, sizeof(pidbuf), ".%d", 12);
    filename += pidbuf;

    filename += ".log";

    return filename;
}