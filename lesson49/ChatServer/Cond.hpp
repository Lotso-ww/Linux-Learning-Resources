#ifndef COND_HPP
#define COND_HPP

#include <iostream>
#include <pthread.h>
#include "Mutex.hpp"

/**
 * @brief 条件变量封装类
 * 核心逻辑：提供线程间的通知机制。
 * 它允许线程在某些条件不满足时挂起，并在其他线程改变条件并发送信号时被唤醒。
 */
class Cond 
{
public:
    // 构造函数：初始化条件变量
    Cond()
    {
        // nullptr 表示使用操作系统默认的条件变量属性
        pthread_cond_init(&cond, nullptr);
    }

    /**
     * @brief 等待条件满足
     * @param mutex 必须是当前线程已经持有的互斥锁
     * * 底层逻辑“三步跳”：
     * 1. 自动释放传入的 mutex 锁（这样其他线程才能修改临界资源）。
     * 2. 将当前线程挂起并加入到该条件变量的等待队列中。
     * 3. 当被唤醒返回时，会自动尝试重新竞争并持有该 mutex 锁。
     */
    void Wait(Mutex &mutex)
    {
        // 调用封装好的 Mutex 类的 Origin() 接口，配合底层 C 接口使用
        pthread_cond_wait(&cond, mutex.Origin());
    }

    // 唤醒一个在此条件变量下等待的线程
    void NotifyOne()
    {
        // 唤醒队列中的第一个线程（如果存在）
        pthread_cond_signal(&cond);
    }

    // 唤醒所有在此条件变量下等待的线程
    void NotifyAll()
    {
        // 广播通知，常用于多个消费者或复杂的资源变动场景
        pthread_cond_broadcast(&cond);
    }

    // 析构函数：销毁条件变量资源
    ~Cond()
    {
        /**
         * 注意事项：
         * 销毁一个仍有线程在等待的条件变量是危险行为。
         * 在线程池销毁前，通常需要先调用 NotifyAll 并回收所有线程。
         */
        pthread_cond_destroy(&cond);
    }

private:
    pthread_cond_t cond; // POSIX 线程库提供的底层条件变量结构
};

#endif
