#include "Thread.hpp"
#include <iostream>
#include <unistd.h>

void hello()
{
    while(true)
    {
        std::cout << "Hello" << std::endl;
        sleep(1);
    }
}

int main()
{
    Thread t(hello);
    t.start();
    // 调用join()等待线程结束
    t.join();
    return 0;
}
