#include <functional>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <vector>
#include "RingQueue.hpp"
#include "Thread.hpp"

// Task -- 面向过程任务
using task_t = std::function<void()>;

void Task()
{
    char name[64];
    pthread_getname_np(pthread_self(), name, sizeof(name));
    std::cout << "我是一个任务, 处理我的是: " << name << std::endl;
}

// 多生产者多消费者
int main()
{
    // 1. 使用RingQueue -- 智能指针
    std::unique_ptr<RingQueue<task_t>> ringqueue = std::make_unique<RingQueue<task_t>>();

    auto Consumer_cb = [&ringqueue]{
        while(true)
        {
            task_t t;
            ringqueue->Pop(&t);

            // 处理任务
            t();
        }
    };

    auto Productor_cb = [&ringqueue]{
        while(true)
        {
            sleep(1);
            ringqueue->EnQueue(Task);
            // 防止混乱,我们这里就不打印生产者的这个了
            // std::cout << "生产了任务" << std::endl;
        }
    };

    // 2. 创建多个线程 -- 这里也优化一下
    const int c_count = 3;
    const int p_count = 3;
    std::vector<Thread> ct;
    std::vector<Thread> pt;
    for(int i = 0; i < c_count; i++)
    {
        Thread c(Consumer_cb);
        ct.push_back(c);
    }
    for(int i = 0; i < p_count; i++)
    {
        Thread p(Productor_cb);
        pt.push_back(p);
    }


    // 3. 启动线程
    for(auto& c : ct)
    {
        c.start();
    }
    for(auto& p : pt)
    {
        p.start();
    }

    // 4. 回收线程
    for(auto& p : pt)
    {
        p.join();
    }
    for(auto& p : pt)
    {
        p.join();
    }

    return 0;
} 