#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&_lock, nullptr);
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&_lock);
    }
    void Lock()
    {
        pthread_mutex_lock(&_lock);
    }
    void UnLock()
    {
        pthread_mutex_unlock(&_lock);
    }
    pthread_mutex_t* Origin()
    {
        return &_lock;
    }
private:
    pthread_mutex_t _lock;
};


class LockGuard
{
public:
    LockGuard(Mutex* lockptr) : _lockptr(lockptr)
    {
        _lockptr->Lock();
    }
    ~LockGuard()
    {
        _lockptr->UnLock();
    }
private:
    Mutex* _lockptr;
};
#endif
