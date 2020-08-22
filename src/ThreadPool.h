#ifndef LOGGER_THREADPOOL_H
#define LOGGER_THREADPOOL_H

#include "Condition.h"
#include "MutexLock.h"
#include "Thread.h"
#include "Types.h"
#include <boost/noncopyable.hpp>

#include <deque>
#include <vector>

class ThreadPool: boost::noncopyable 
{
    public:
        typedef std::function<void ()> Task;
    private:
        mutable MutexLock mutex_;
        /**
         * notEmpty_ 代表队列不为空的变量的条件
         *  队列为空，阻塞当前线程，等待任务队列不为空
         *  在队列不为空，用于唤醒线程处理队列任务
         */
        Condition notEmpty_;
        /**
         * notFull_ 代表队列未满
         *  队列已满，阻塞当前队列，等待任务队列被处理
         *  队列未满，用于唤醒线程处理队列任务
         */
        Condition notFull_;
        string name_;
        Task threadInitCallback_;
        std::vector<std::unique_ptr<Thread>> threads_;
        std::deque<Task> queue_;
        size_t maxQueueSize_;
        bool running_;
    
    public:
        explicit ThreadPool(const string& nameArg = string("ThreadPool"));
        ~ThreadPool();

        // Must be called before start()
        void setMaxQueueSize(int maxSize)
        {
            maxQueueSize_ = maxSize;
        }

        void setThreadInitCallback(const Task& cb)
        {
            threadInitCallback_ = cb;
        }

        void start(int numThreads);
        void stop();

        const string& name() const
        {
            return name_;
        }

        size_t queueSize() const;

        /**
         * 如果maxQueueSize>0，则可能会阻止
         * stop（）之后的调用将立即返回
         */
        void run(Task f);

    private:
        bool isFull() const;
        void runInThread();
        Task take();

};

#endif