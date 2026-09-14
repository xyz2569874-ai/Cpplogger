#pragma once
#include <iostream>
#include <string>
#include <pthread.h>
namespace LockModule
{

    // 对锁进⾏封装，可以独⽴使⽤

    class Mutex
    {
    public:
        // 删除不要的拷⻉和赋值

        Mutex(const Mutex &) = delete;
        const Mutex &operator=(const Mutex &) = delete;
        Mutex()
        {
            int n = pthread_mutex_init(&_mutex, nullptr);
            (void)n;
        }
        void Lock()
        {
            int n = pthread_mutex_lock(&_mutex);
            (void)n;
        }
        void Unlock()
        {
            int n = pthread_mutex_unlock(&_mutex);
            (void)n;
        }
        pthread_mutex_t *GetMutexOriginal() //

        {
            return &_mutex;
        }
        ~Mutex()
        {
            int n = pthread_mutex_destroy(&_mutex);
            (void)n;
        }

    private:
        pthread_mutex_t _mutex;
    };

    class LockGuard
    {
    public:
        LockGuard(Mutex &mutex) : _mutex(mutex)
        {
            _mutex.Lock();
        }
        ~LockGuard()
        {
            _mutex.Unlock();
        }

    private:
        Mutex &_mutex;
    };
}