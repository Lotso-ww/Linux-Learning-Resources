#pragma once

#include <iostream>
#include <vector>
#include "Sem.hpp"
#include "Mutex.hpp"

const static int gdefaultcap = 5;

template<class T>
class RingQueue
{
public:
    RingQueue(int cap = gdefaultcap): _ringqueue(cap), _cap(cap), _c_step(0), _p_step(0)
                                    , _data_sem(0), _space_sem(cap) // 用我们自己的就得在这里初始化一下了,有个参数
    {

    }
    void EnQueue(const T &in)
    {
        // 思考一个问题,是先加锁还是先信号量? -- 先信号量
        // 申请信号量是对资源的预定机制，买票；申请锁就是买票时候排队的过程，那我难道每次看电影还得先排队再买票啊，效率太低了。
        // 我们现实生活中现在都是网上买票，我想去看电影再去排队。别让其他线程闲着啊，先预定了资源再说呗，后面再一个个进来，这样是不是效率高点啊。
        // 这个的前提其实是因为我们的信号量PV操作是原子的
        // 生产者
        _space_sem.P(); // 空间P()  --;

        {
            LockGuard pLockGuard(&_p_mutex);
            _ringqueue[_p_step] = in;
            _p_step++;
            _p_step %= _cap; // 模运算回来
        }

        _data_sem.V(); // 数据V()  ++;
    }
    void Pop(T *out)
    {
        // 消费者
        _data_sem.P(); // 数据P()  --;

       // 把这个区域直接括起来
       {
            LockGuard cLockGuard(&_c_mutex);
            *out = _ringqueue[_c_step];
            _c_step++;
            _c_step %= _cap;
       }

        _space_sem.V(); // 空间V()  ++;
    }
    ~RingQueue()
    {}
private:
    std::vector<T> _ringqueue;
    int _cap; // 容量

    // 两个下标
    int _c_step;
    int _p_step;

    // 两个信号量, 消费者关心数据, 生产者关心空间
    Sem _data_sem;
    Sem _space_sem;

    // 两个锁 -- 单生产单消费者模型用不上
    // 但是多生产者多消费者模型用的上
    Mutex _p_mutex;
    Mutex _c_mutex;
};