#ifndef LOGGER_MUTEXLOCK_H
#define LOGGER_MUTEXLOCK_H

#include <pthread.h>
#include <cstdio>
#include <assert.h>
#include "CurrentThread.h"
#include <boost/noncopyable.hpp>

class MutexLock: boost::noncopyable 
{
    private:
        pthread_mutex_t mutex;
        pid_t holder_;

    public:
        MutexLock() 
        {
            pthread_mutex_init(&mutex, NULL);
        }

        ~MutexLock()
        {
            pthread_mutex_lock(&mutex);
            pthread_mutex_destroy(&mutex);
        }

        bool isLockedByThisThread() const
        {
            return holder_ == CurrentThread::tid(); 
        }

        void assertLocked() const
        {
            assert(isLockedByThisThread());
        }

        void lock()
        {
            pthread_mutex_lock(&mutex);
            assignHolder();
        }

        void unlock()
        {
            unassignHolder();
            pthread_mutex_unlock(&mutex);
        }

        pthread_mutex_t *get()
        {
            return &mutex;
        }
    // 友元类不受访问权限影响
    private:
        friend class Condition;

        void unassignHolder()
        {
            holder_ = 0;
        }

        void assignHolder()
        {
            holder_ = CurrentThread::tid();
        }
};

class MutexLockGuard: boost::noncopyable
{
    private:
        MutexLock &mutex;
    public:
        explicit MutexLockGuard(MutexLock &_mutex): mutex(_mutex)
        {
            mutex.lock();
        }

        ~MutexLockGuard()
        {
            mutex.unlock();
        }
};

#endif