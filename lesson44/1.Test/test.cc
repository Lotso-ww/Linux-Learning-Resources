#include <iostream>
#include <thread>
#include <unistd.h>
#include <pthread.h>
#include <string>

int tickets = 1000;
pthread_mutex_t glock;
pthread_cond_t gcond;

void grabTicket(const std::string& name) 
{
    while (true) 
    {
        pthread_mutex_lock(&glock);
        pthread_cond_wait(&gcond, &glock);
        if (tickets > 0) 
        {
            usleep(1000);
            std::cout << name << " grabbed ticket, remaining: " << --tickets << std::endl;
            pthread_mutex_unlock(&glock);
        } else 
        {
            pthread_mutex_unlock(&glock);
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

    // 唤醒线程
    while(true)
    {
        std::cout << "wake up ..." << std::endl;
        pthread_cond_signal(&gcond); // 也可以去唤醒一批
        sleep(1);
    }

    // 等待所有线程结束
    for (int i = 0; i < THREAD_COUNT; i++) 
    {
        threads[i].join();
    }
    return 0;
}
