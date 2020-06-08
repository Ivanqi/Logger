#ifndef LOGGER_FILEUTIL_H
#define LOGGER_FILEUTIL_H

#include <boost/noncopyable.hpp>
#include <string>

class AppendFile: boost::noncopyable
{
    private:
        size_t write(const char *logline, size_t len);
        FILE* fp_;
        char buffer_ [60 * 1024];
    public:
        explicit AppendFile(std::string filename);

        ~AppendFile();

        // append 往文件中写
        void append(const char *logline, const size_t len);

        void flush();
};

#endif