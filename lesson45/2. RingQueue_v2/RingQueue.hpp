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

        // 初始化两个锁
        pthread_mutex_init(&_c_mutex, nullptr);
        pthread_mutex_init(&_p_mutex, nullptr);
    }
    void EnQueue(const T &in)
    {
        // 思考一个问题,是先加锁还是先信号量? -- 先信号量
        // 申请信号量是对资源的预定机制，买票；申请锁就是买票时候排队的过程，那我难道每次看电影还得先排队再买票啊，效率太低了。
        // 我们现实生活中现在都是网上买票，我想去看电影再去排队。别让其他线程闲着啊，先预定了资源再说呗，后面再一个个进来，这样是不是效率高点啊。
        // 这个的前提其实是因为我们的信号量PV操作是原子的
        // 生产者
        sem_wait(&_space_sem); // 空间P()  --;

        pthread_mutex_lock(&_p_mutex);
        _ringqueue[_p_step] = in;
        _p_step++;
        _p_step %= _cap; // 模运算回来
        pthread_mutex_unlock(&_p_mutex);

        sem_post(&_data_sem); // 数据V()  ++;
    }
    void Pop(T *out)
    {
        // 消费者
        sem_wait(&_data_sem); // 数据P()  --;

        pthread_mutex_lock(&_c_mutex);
        *out = _ringqueue[_c_step];
        _c_step++;
        _c_step %= _cap;
        pthread_mutex_unlock(&_c_mutex);

        sem_post(&_space_sem); // 空间V()  ++;
    }
    ~RingQueue()
    {
        // 销毁两个信号量
        sem_destroy(&_data_sem);
        sem_destroy(&_space_sem);

        // 销毁两个锁
        pthread_mutex_destroy(&_c_mutex);
        pthread_mutex_destroy(&_p_mutex);
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

    // 两个锁 -- 单生产单消费者模型用不上
    // 但是多生产者多消费者模型用的上
    pthread_mutex_t _p_mutex;
    pthread_mutex_t _c_mutex;
};
