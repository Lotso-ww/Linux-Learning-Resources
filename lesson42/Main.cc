#include <iostream>
#include <unistd.h>
#include <vector>
#include "Thread.hpp"

void hello()
{
    int count = 0;
    while(count < 5) // 执行5次后退出
    {
        std::cout << "hello thread" << std::endl;
        sleep(1);
        count++;
    }
}
int main()
{
    std::vector<Thread> threads;
    for(int i = 0; i < 10; i++)
    {
        threads.emplace_back(hello);
    }

    for(auto& t : threads)
    {
        t.start();
    }

    for(auto& t : threads)
    {
        t.join();
    }
    return 0;
}