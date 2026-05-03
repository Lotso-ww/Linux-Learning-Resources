// #include <iostream>
// #include <thread>
// #include <unistd.h>
// #include <mutex>
// #include <string>
// #include "Mutex.hpp"

// int tickets = 1000;
// Mutex lock;
// // std::mutex mutex;   

// void grabTicket(const std::string& name) 
// {
//     while (true) 
//     {
//         lock.Lock();
//         // mutex.lock();
//         if (tickets > 0) 
//         {
//             usleep(1000);
//             std::cout << name << " grabbed ticket, remaining: " << --tickets << std::endl;
//             // mutex.unlock();
//             lock.UnLock();
//         } else 
//         {
//             // mutex.unlock();
//             lock.UnLock();
//             break;
//         }
//     }
// }

// int main() 
// {
//     const int THREAD_COUNT = 4;
//     std::thread threads[THREAD_COUNT]; // 创建一个数组去存线线程
    
//     // 创建线程
//     for (int i = 0; i < THREAD_COUNT; i++) 
//     {
//         std::string name = "Thread-" + std::to_string(i + 1);
//         threads[i] = std::thread(grabTicket, name);
//     }

//     // 等待所有线程结束
//     for (int i = 0; i < THREAD_COUNT; i++) 
//     {
//         threads[i].join();
//     }
//     return 0;
// }


// RAII模式
#include <iostream>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <string>
#include "Mutex.hpp"

int tickets = 1000;
Mutex lock;
// std::mutex mutex;   

void grabTicket(const std::string& name) 
{
    while (true) 
    {
        LockGuard lockGuard(&lock);
        // std::lock_guard<std::mutex> lockGuard(mutex);
        if (tickets > 0) 
        {
            usleep(1000);
            std::cout << name << " grabbed ticket, remaining: " << --tickets << std::endl;
        } else 
        {
            break;
        }
    }
}

int main() 
{
    const int THREAD_COUNT = 4;
    std::thread threads[THREAD_COUNT]; // 创建一个数组去存线线程
    
    // 创建线程
    for (int i = 0; i < THREAD_COUNT; i++) 
    {
        std::string name = "Thread-" + std::to_string(i + 1);
        threads[i] = std::thread(grabTicket, name);
    }

    // 等待所有线程结束
    for (int i = 0; i < THREAD_COUNT; i++) 
    {
        threads[i].join();
    }
    return 0;
}
