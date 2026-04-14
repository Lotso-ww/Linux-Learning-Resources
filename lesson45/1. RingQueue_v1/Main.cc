#include <iostream>
#include <memory>
#include "RingQueue.hpp"
#include "Thread.hpp"


int main()
{
    // 1. 使用RingQueue -- 智能指针
    std::unique_ptr<RingQueue<int>> ringqueue = std::make_unique<RingQueue<int>>();

    // 2. 创建线程, 把Lambda表达式用起来
    Thread c([&ringqueue]{
        while(true)
        {
            int data = 0;
            ringqueue->Pop(&data);
            std::cout << "消费了数据: " << data << std::endl;
        }
    });
    Thread p([&ringqueue]{
        int data = 0;
        while(true)
        {
            sleep(1);
            ringqueue->EnQueue(data);
            std::cout << "生产了数据: " << data << std::endl;
            data++;
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