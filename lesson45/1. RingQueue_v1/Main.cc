#include <functional>
#include <iostream>
#include <memory>
#include <pthread.h>
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

int main()
{
    // 1. 使用RingQueue -- 智能指针
    std::unique_ptr<RingQueue<task_t>> ringqueue = std::make_unique<RingQueue<task_t>>();

    // 2. 创建线程, 把Lambda表达式用起来
    Thread c([&ringqueue]{
        while(true)
        {
            task_t t;
            ringqueue->Pop(&t);

            // 处理任务
            t();
        }
    });
    Thread p([&ringqueue]{
        while(true)
        {
            sleep(1);
            ringqueue->EnQueue(Task);
            std::cout << "生产了任务" << std::endl;
        }
    });

    // 3. 启动线程
    c.start();
    p.start();

    // 4. 回收线程
    c.join();
    p.join();
    return 0;
} 

// int 类型 -- 测试下两个场景
// int main()
// {
//     // 1. 使用RingQueue -- 智能指针
//     std::unique_ptr<RingQueue<int>> ringqueue = std::make_unique<RingQueue<int>>();

//     // 2. 创建线程, 把Lambda表达式用起来
//     Thread c([&ringqueue]{
//         while(true)
//         {
//             int data = 0;
//             ringqueue->Pop(&data);
//             std::cout << "消费了数据: " << data << std::endl;
//         }
//     });
//     Thread p([&ringqueue]{
//         int data = 0;
//         while(true)
//         {
//             sleep(1);
//             ringqueue->EnQueue(data);
//             std::cout << "生产了数据: " << data << std::endl;
//             data++;
//         }
//     });

//     // 3. 启动线程
//     c.start();
//     p.start();

//     // 4. 回收线程
//     c.join();
//     p.join();
//     return 0;
// } 