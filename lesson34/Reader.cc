#include "Shm.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

// // RAII
// Shm shm;
// class Init
// {
// public:
//     Init()
//     {
        
//     }
//     ~Init()
//     {
//         std::cout << "~Init()" << std::endl;
//     }
// };
// Writer -> shm -> Reader
int main()
{
    // 在内核中创建共享内存
    Shm shm;
    shm.Create();
    char *addr = (char*)shm.Attch();


    buffer_t *shmbuf = (buffer_t*)addr;
    int old = shmbuf->count; // 实现一个简单的保护和同步机制
    while(true)
    {
        if(old != shmbuf->count)
        {
            std::cout << "count: " << shmbuf->count << std::endl;
            std::cout << "data: " << shmbuf->buf << std::endl;
            old = shmbuf->count;
        }
        usleep(74329);

        if(shmbuf->count >= 26)
        {
            break;
        }
    }
    
    // shm.Debug();
    // shm.GetShmAttr();

    // 删除共享内存   shm.Detach();
    shm.Delete();

    return 0;
}
