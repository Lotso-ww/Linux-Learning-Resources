#pragma once

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

const static int gdefaultcap = 5;

template<class T>
class RingQueue
{
public:
    RingQueue(int cap = gdefaultcap):_ringqueue(cap), _cap(cap), _c_step(0), _p_step(0)
    {
        // 初始化两个信号量
        sem_init(&_data_sem, 0, 0);
        sem_init(&_space_sem, 0, cap); // 初始空间信号量是cap
    }
    void EnQueue(T &in)
    {
        // 生产者
        sem_wait(&_space_sem); // 空间P()  --;

        _ringqueue[_p_step] = in;
        _p_step++;
        _p_step %= _cap; // 模运算回来

        sem_post(&_data_sem); // 数据V()  ++;
    }
    void Pop(T *out)
    {
        // 消费者
        sem_wait(&_data_sem); // 数据P()  --;

        *out = _ringqueue[_c_step];
        _c_step++;
        _c_step %= _cap;

        sem_post(&_space_sem); // 空间V()  ++;
    }
    ~RingQueue()
    {
        // 销毁两个信号量
        sem_destroy(&_data_sem);
        sem_destroy(&_space_sem);
    }
private:
    std::vector<T> _ringqueue;
    int _cap; // 容量

    // 两个下标
    int _c_step;
    int _p_step;

    // 两个信号量, 消费者关心数据, 生产者关心空间
    sem_t _data_sem;
    sem_t _space_sem;

    // 两个锁
    pthread_mutex_t _p_mutex;
    pthread_mutex_t _c_mutex;
};