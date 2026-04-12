#ifndef BLOCKQUEUE_HPP
#define BLOCKQUEUE_HPP
#include <iostream>
#include <pthread.h>
#include <queue>

template <class T>
class BlockQueue 
{
public:
    BlockQueue(int cap = 5): _cap(cap), _consumer_sleep_cnt(0), _productor_sleep_cnt(0), _low_water(cap / 2), _high_water(cap / 3 * 2)
    {
        // 锁 && 两个条件变量 -- 初始化
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond_consumer, nullptr);
        pthread_cond_init(&_cond_productor, nullptr);
    }
    void Push(const T& in)
    {
        // 生产者
        pthread_mutex_lock(&_mutex);
        // 1. 判断是否为满本质也是临界区
        // 2. 那么判断本身也一定是在加锁和解锁之间
        // 3. 那么判断结果的处理，也必然在临界区内部
        while(_queue.size() == _cap) // 选择while循环判断避免出现函数调用失败和伪唤醒的问题
        {
            // 4. 等待的本质, 核心操作就是把线程设挂起到cond的等待队列中
            // 结论：这个函数在底层会原子性的自动释放第二个参数对应的锁。不然我带着锁走,我的消费者永远申请不到锁,我也就不可能再被唤醒了,死锁了。
            // a.函数调用失败 b. 伪唤醒
            _productor_sleep_cnt++;
            pthread_cond_wait(&_cond_productor, &_mutex);
            _productor_sleep_cnt--;
            // 5. 当线程被唤醒时, 也必然是在临界区内部被唤醒
            // 结论：pthread_cond_wait对应的线程在被唤醒的时候, 自动重新申请锁,锁申请成功这个函数才会返回
        }
        _queue.push(in);
        // 满足条件去唤醒,但是我们为了方便实验观察就把这里先注释掉直接无脑唤醒
        // 生产者这里去唤醒消费者
        // if(_consumer_sleep_cnt > 0 && _queue.size() > _high_water)
        //     pthread_cond_signal(&_cond_consumer);
        pthread_cond_signal(&_cond_consumer); // 在解锁之前和之后都可以,因为都会去申请锁
        pthread_mutex_unlock(&_mutex);
    }
    void Pop(T* out)
    {
        // 消费者
        // 用的都是同一个锁
        pthread_mutex_lock(&_mutex);
        while(_queue.empty())
        {
            _consumer_sleep_cnt++;
            pthread_cond_wait(&_cond_consumer, &_mutex);
            _consumer_sleep_cnt--;
        }
        *out = _queue.front();
        _queue.pop();
        // 消费者这里去唤醒生产者
        // if(_productor_sleep_cnt > 0 && _queue.size() < _low_water)
        //     pthread_cond_signal(&_cond_productor);
        pthread_cond_signal(&_cond_productor);
        pthread_mutex_unlock(&_mutex);
    }
    ~BlockQueue()
    {
        // 锁 && 两个条件变量 -- 销毁
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond_consumer);
        pthread_cond_destroy(&_cond_productor);
    }
private:
    int _cap; // 记录容量
    // 条件变量
    pthread_cond_t _cond_consumer; // empty
    pthread_cond_t _cond_productor; // full

    // 唤醒条件
    int _consumer_sleep_cnt;
    int _productor_sleep_cnt;

    // 高低水位
    unsigned int _low_water;
    unsigned int _high_water;

    std::queue<T> _queue;
    pthread_mutex_t _mutex;

};

#endif