#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FileUtil.h"

AppendFile::AppendFile(std::string filename): fp_(fopen(filename.c_str(), "ae"))
{
    // 用户提供缓冲区
    setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
    fclose(fp_);
}

void  AppendFile::append(const char* logline, const size_t len)
{

    size_t n = this->write(logline, len);
    size_t remain = len - n;

    // 如果没有写入全部字节，则继续写入
    while (remain > 0) {
        size_t x = this->write(logline + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed ! \n");
            }
            break;
        }
        n += x;
        remain = len - n;
    }

    writtenBytes_ += len;
}

void AppendFile::flush()
{
    fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
    return fwrite_unlocked(logline, 1, len, fp_);
}