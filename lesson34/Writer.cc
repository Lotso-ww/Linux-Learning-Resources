// header only
#include "Shm.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

// Write.
int main()
{
    // 在共享内存中获取共享内存
    Shm shm;
    shm.Get();
    shm.Debug();

    return 0;
}