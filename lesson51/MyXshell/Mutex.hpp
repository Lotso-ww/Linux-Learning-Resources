#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <iostream>
#include <pthread.h>

// 互斥锁封装类：提供加锁/解锁及获取原始锁的接口
class Mutex
{
public:
    // 构造函数：初始化互斥锁
    Mutex()
    {
        pthread_mutex_init(&_lock, nullptr);
    }
    // 析构函数：销毁互斥锁
    ~Mutex()
    {
        pthread_mutex_destroy(&_lock);
    }
    // 加锁操作
    void Lock()
    {
        pthread_mutex_lock(&_lock);
    }
    // 解锁操作
    void UnLock()
    {
        pthread_mutex_unlock(&_lock);
    }
    // 获取原始互斥锁指针，用于需要原生 pthread_mutex_t 的接口
    pthread_mutex_t* Origin()
    {
        return &_lock;
    }
private:
    pthread_mutex_t _lock;  // POSIX 互斥锁
};

// RAII 风格的锁守卫类：构造时加锁，析构时解锁，自动管理锁的生命周期
class LockGuard
{
public:
    // 构造函数：接收一个 Mutex 指针，并立即加锁
    LockGuard(Mutex* lockptr) : _lockptr(lockptr)
    {
        _lockptr->Lock();
    }
    // 析构函数：自动解锁
    ~LockGuard()
    {
        _lockptr->UnLock();
    }
private:
    Mutex* _lockptr;  // 指向被管理的互斥锁
};

#endif
