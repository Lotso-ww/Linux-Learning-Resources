#include "Shm.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

// Writer -> shm -> Reader
int main()
{
    // 在内核中创建共享内存
    Shm shm;
    shm.Create();

    sleep(3);
    shm.Attch();
    
    shm.Debug();
    shm.GetShmAttr();

    sleep(5);
    // 删除共享内存
    shm.Delete();

    return 0;
}
