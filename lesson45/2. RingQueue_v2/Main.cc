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
    // 2. 创建多个线程
    Thread c1(Consumer_cb);
    Thread c2(Consumer_cb);
    Thread c3(Consumer_cb);

    Thread p1(Productor_cb);
    Thread p2(Productor_cb);
    Thread p3(Productor_cb);

    // 3. 启动线程
    c1.start();
    c2.start();
    c3.start();
    p1.start();
    p2.start();
    p3.start();

    // 4. 回收线程
    c1.join();
    c2.join();
    c3.join();
    p1.join();
    p2.join();
    p3.join();

    return 0;
} 