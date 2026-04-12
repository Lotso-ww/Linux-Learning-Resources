#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "BlockQueue.hpp"

// 消费
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

// 生产
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

// 测试单生产者单消费者模型
int main()
{
    // 1. 创建出一个交易场所
    BlockQueue<int> *bq = new BlockQueue<int>();
    // 2. 创建出两个角色
    pthread_t c, p;
    pthread_create(&c, nullptr, Consumer, bq);
    pthread_create(&p, nullptr, Productor, bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    delete bq;
    return 0;
}