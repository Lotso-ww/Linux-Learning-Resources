// #include <iostream>
// #include <thread>
// #include <unistd.h>

// // C++中的线程
// void hello()
// {
//     while(true)
//     {
//         std::cout << "我是新进程..., pid: " << getpid() << std::endl;
//         sleep(1);
//     }
// }


// int main()
// {
//     std::thread t(hello);

//     while(true)
//     {
//         std::cout << "我是主线程..., pid: " << getpid() << std::endl;
//         sleep(1);
//     }

//     t.join();
//     return 0;
// }

#include <iostream>
#include <pthread.h>
#include <unistd.h>

// Linux中封装的线程 -- 其实Linux是只有轻量级进程的概念的
void *hello(void *args)
{
    while(true)
    {
        const char *name = (const char*)args;
        std::cout << "我是新线程..., pid: " << getpid() << " name is : "<< name <<std::endl;
        sleep(1);
    }
}


int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, hello, (void*)"new-thread");

    while(true)
    {
        std::cout << "我是主线程..., pid: " << getpid() << std::endl;
        sleep(1);
    }

    return 0;
}