#ifndef LOGGER_THREAD_H
#define LOGGER_THREAD_H

#include <Types.h>
#include <functional>
#include <memory>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <boost/noncopyable.hpp>

#include "CountDownLatch.h"

class Thread: boost::noncopyable
{
    public:
        typedef std::function<void()> ThreadFunc;

    private:
        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        pid_t tid_;
        ThreadFunc func_;
        string name_;
        CountDownLatch latch_;

    public:

        explicit Thread (const ThreadFunc&, const string& name = string());

        ~Thread();
        
        void start();

        int join();

        bool started() const {
            return started_;
        };

        pid_t tid() const
        {
            return tid_;
        }

        const string& name() const
        {
            return name_;
        }
    private:
        void setDefaultName();
};

#endif