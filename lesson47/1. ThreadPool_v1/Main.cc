#include "Logger.hpp"
#include "Task.hpp"
#include "Threadpool.hpp"
#include <memory>
#include <unistd.h>

int main()
{
    ENABLE_CONSOLE_LOG_STRATEGY();

    std::unique_ptr<ThreadPool<task_t>> tp = std::make_unique<ThreadPool<task_t>>();
    tp->Start();

    int cnt = 10;
    while(cnt--)
    {
        LOG(LogLevel::DEBUG) << "-----------------------: " << cnt;
        sleep(1);
        tp->Enqueue(task1); 

        sleep(1);
        tp->Enqueue(task2);
    }

    tp->Stop();
    tp->Wait();
}