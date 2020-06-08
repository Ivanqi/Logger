#ifndef LOGGER_MUTEXLOCK_H
#define LOGGER_MUTEXLOCK_H

#include <pthread.h>
#include <cstdio>
#include <boost/noncopyable.hpp>

class MutexLock: boost::noncopyable 
{
    private:
        pthread_mutex_t mutex;
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

        void lock()
        {
            pthread_mutex_lock(&mutex);
        }

        void unlock()
        {
            pthread_mutex_unlock(&mutex);
        }

        pthread_mutex_t *get()
        {
            return &mutex;
        }
    // 友元类不受访问权限影响
    private:
        friend class Condition;
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