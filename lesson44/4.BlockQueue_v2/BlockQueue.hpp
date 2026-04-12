#ifndef BLOCKQUEUE_HPP
#define BLOCKQUEUE_HPP
#include "Cond.hpp"
#include "Mutex.hpp"
#include <iostream>
#include <pthread.h>
#include <queue>

template <class T>
class BlockQueue 
{
public:
    BlockQueue(int cap = 5): _cap(cap), _consumer_sleep_cnt(0), _productor_sleep_cnt(0), _low_water(cap / 2), _high_water(cap / 3 * 2)
    {}
    void Push(const T& in)
    {
        // 生产者
        // RAII模式
        // 我们也可以加个大扩号扩起来这些,更明显
        LockGuard lockGuard(&_mutex); // 加锁
        while(_queue.size() == _cap) // 选择while循环判断避免出现函数调用失败和伪唤醒的问题
        {
            _productor_sleep_cnt++;
            _cond_productor.Wait(_mutex);
            _productor_sleep_cnt--;
        }
        _queue.push(in);
        // if(_consumer_sleep_cnt > 0 && _queue.size() > _high_water)
        //     pthread_cond_signal(&_cond_consumer);
        _cond_consumer.NotifyOne();// 在解锁之前和之后都可以,因为都会去申请锁
    }
    void Pop(T* out)
    {
        // 消费者
        // 用的都是同一个锁
        LockGuard lockGuard(&_mutex);
        while(_queue.empty())
        {
            _consumer_sleep_cnt++;
           _cond_consumer.Wait(_mutex);
            _consumer_sleep_cnt--;
        }
        *out = _queue.front();
        _queue.pop();
        // 消费者这里去唤醒生产者
        // if(_productor_sleep_cnt > 0 && _queue.size() < _low_water)
        //     pthread_cond_signal(&_cond_productor);
        _cond_productor.NotifyOne();
    }
    ~BlockQueue()
    {}
private:
    int _cap; // 记录容量
    // 条件变量
    Cond _cond_consumer; // empty
    Cond _cond_productor; // full

    // 唤醒条件
    int _consumer_sleep_cnt;
    int _productor_sleep_cnt;

    // 高低水位
    unsigned int _low_water;
    unsigned int _high_water;

    std::queue<T> _queue;
    Mutex _mutex;

};

#endif