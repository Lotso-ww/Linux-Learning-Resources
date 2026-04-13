#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "BlockQueue.hpp"
#include "Task.hpp"

// 消费 - 处理int类型
void* Consumer(void* args)
{
    BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
    while(true)
    {
        // sleep(1);
        int data = 0;
        bq->Pop(&data);
        std::cout << "消费了数据: " << data << std::endl;
    }
    // return nullptr;
}

// 生产 - 处理int类型
void* Productor(void* args)
{
     BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
     int data = 10;
    while(true)
    {
        sleep(1);
        bq->Push(data);
        std::cout << "生产了数据: " << data << std::endl;
        ++data;
    }
    // return nullptr;
}

// // 测试单生产者单消费者模型 - int类型
// int main()
// {
//     // 1. 创建出一个交易场所
//     BlockQueue<int> *bq = new BlockQueue<int>();
//     // 2. 创建出两个角色
//     pthread_t c, p;
//     pthread_create(&c, nullptr, Consumer, bq);
//     pthread_create(&p, nullptr, Productor, bq);

//     pthread_join(c, nullptr);
//     pthread_join(p, nullptr);
//     delete bq;
//     return 0;
// }

// 测试多生产者多消费者模型 - int类型
int main()
{
    // 定义线程数量
    const int PRODUCER_COUNT = 2; // 2个生产者
    const int CONSUMER_COUNT = 3; // 3个消费者
    
    // 1. 创建出一个交易场所
    BlockQueue<int> *bq = new BlockQueue<int>();
    
    // 2. 创建出多个角色
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];
    
    // 创建生产者线程
    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        pthread_create(&producers[i], nullptr, Productor, bq);
    }
    
    // 创建消费者线程
    for (int i = 0; i < CONSUMER_COUNT; ++i) {
        pthread_create(&consumers[i], nullptr, Consumer, bq);
    }

    // 等待所有线程结束
    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        pthread_join(producers[i], nullptr);
    }
    for (int i = 0; i < CONSUMER_COUNT; ++i) {
        pthread_join(consumers[i], nullptr);
    }
    
    delete bq;
    return 0;
}


// // 消费 - 处理Task类型
// void* Consumer(void* args)
// {
//     BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
//     while(true)
//     {
//         // sleep(1);
//         Task task;
//         bq->Pop(&task);
//         std::cout << "消费了任务: " << task.GetTaskRes() << task.GetResult() << std::endl;
//     }
//     // return nullptr;
// }

// // 生产 - 处理Task类型
// void* Productor(void* args)
// {
//      BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
//     while(true)
//     {
//         sleep(1);
//         // 生成随机任务
//         int a = rand() % 100;
//         int b = rand() % 100;
//         char op = "+-*/"[rand() % 4];
//         Task task(a, b, op);
//         bq->Push(task);
//         std::cout << "生产了任务: " << task.GetTaskRes() << "?" << std::endl;
//     }
//     // return nullptr;
// }

// // 测试Task任务的阻塞队列
// int main()
// {
//     // 初始化随机数种子
//     srand(time(nullptr));
    
//     // 1. 创建出一个交易场所
//     BlockQueue<Task> *bq = new BlockQueue<Task>();
//     // 2. 创建出两个角色
//     pthread_t c, p;
//     pthread_create(&c, nullptr, Consumer, bq);
//     pthread_create(&p, nullptr, Productor, bq);

//     pthread_join(c, nullptr);
//     pthread_join(p, nullptr);
//     delete bq;
//     return 0;
// }