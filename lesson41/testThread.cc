// // 测试线程创建
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>
// void* threadrun(void* arg)
// {
//     std::string name = static_cast<const char*>(arg);
//     while(true)
//     {
//         printf("我是一个新线程, tid: %lu, pid: %d\n", pthread_self(), getpid());
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, NULL, threadrun, (void*)"thread-1");

//     while(true)
//     {
//         printf("创建新进程成功, new tid: %lu, main tid: %lu, pid: %d\n", tid, pthread_self(), getpid());
//         sleep(1);
//     }
//     return 0;
// }

// // 测试创建多线程
// #include <iostream>
// #include <cstdio>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>
// #include <vector>

// int g_size = 64;
// void* threadrun(void* arg)
// {
//     std::string name = static_cast<const char*>(arg);
//     delete[] (char*)arg;
//     while(true)
//     {
//         printf("我是一个新线程, tid: %lu, pid: %d, name: %s\n", pthread_self(), getpid(), name.c_str());  
//         sleep(1);
//     }
//     return nullptr;
// }

// // ./CreateThread threadnum;
// int main(int argc, char* argv[])
// {
//     if(argc != 2)
//     {
//         printf("Usage: %s threadnum\n", argv[0]);
//         return -1;
//     }
//     int threadnum = std::stoi(argv[1]);
//     std::vector<pthread_t> tids;
//     for(int i = 0; i < threadnum; i++)
//     {
//         pthread_t tid;
//         // char threadname[g_size]; // 这个是不太行的, 如果要让每个线程都有自己的名称, 则需要动态分配内存, 不能使用栈内存
//         char* threadname = (char*)malloc(g_size);
//         snprintf(threadname, g_size, "thread-%d", i + 1);
//         pthread_create(&tid, NULL, threadrun, (void*)threadname);
//         // sleep(1); 这个是不可以完全解决这个问题的
//         tids.push_back(tid);
//     }

//     sleep(10);
//     for(auto& tid : tids)
//     {
//         printf("main for 创建新进程成功, new tid: %lu, main tid: %lu, pid: %d\n", tid, pthread_self(), getpid());
//     }

//     // 主线程
//     while(true)
//     {
//         std::cout << "main thread is running..." << std::endl;
//         sleep(1);
//     }

//     return 0;
// }

// // 构建一个任务进行测试
// #include <iostream>
// #include <cstdio>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>
// #include <vector>
// #include "Task.hpp"

// int g_size = 64;
// void* threadrun(void* arg)
// {
//     Task *t = static_cast<Task*>(arg);
//     sleep(1);
//     (*t)();
//     sleep(1);

//     while(true)
//     {
//         sleep(1);
//         // printf("我是一个新线程, tid: %lu, pid: %d, name: %s\n", pthread_self(), getpid(), name.c_str());  
//         // sleep(1);
//     }
//     return nullptr;
// }

// // ./CreateThread threadnum;
// int main(int argc, char* argv[])
// {
//     if(argc != 2)
//     {
//         printf("Usage: %s threadnum\n", argv[0]);
//         return -1;
//     }
//     int threadnum = std::stoi(argv[1]);
//     std::vector<pthread_t> tids;
//     for(int i = 0; i < threadnum; i++)
//     {
//         pthread_t tid;
//         char threadname[g_size]; // 这个是不太行的, 如果要让每个线程都有自己的名称, 则需要动态分配内存, 不能使用栈内存
//         // char* threadname = (char*)malloc(g_size);
//         snprintf(threadname, g_size, "thread-%d", i + 1);

//         Task *t = new Task(threadname, 10 + i, 20 * i);
//         pthread_create(&tid, NULL, threadrun, (void*)t);
//         tids.push_back(tid);
//         sleep(1);
//     }

//     sleep(10);
//     for(auto& tid : tids)
//     {
//         printf("main for 创建新进程成功, new tid: %lu, main tid: %lu, pid: %d\n", tid, pthread_self(), getpid());
//     }

//     // 主线程
//     while(true)
//     {
//         std::cout << "main thread is running..." << std::endl;
//         sleep(1);
//     }

//     return 0;
// }


// // 测试线程退出
// // 1. return 退出线程函数
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>

// const int g_size = 64;
// void* threadrun(void* arg)
// {
//     std::string name = static_cast<const char*>(arg);
//     int cnt = 5;
//     while(cnt)
//     {
//         printf("我是一个新进程: tid: 0x%lx, pid: %d, name: %s, cnt: %d\n", 
//                 pthread_self(), getpid(), name.c_str(), cnt);
//         cnt--;
//         sleep(1);
//         return nullptr;
//     }
//     // 新进程退出
//     // 只要自己的线程函数跑完,线程自然退出了 -- 调用return,表示线程退出
//     // return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     char threadname[g_size]; 
//     snprintf(threadname, g_size, "thread-%d", 1);
//     pthread_create(&tid, NULL, threadrun, (void*)threadname);

//     while(true)
//         pause();

    
//     return 0;
// }

// // 2. pthread_exit() 退出线程函数
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>

// const int g_size = 64;
// void* threadrun(void* arg)
// {
//     std::string name = static_cast<const char*>(arg);
//     int cnt = 5;
//     while(cnt)
//     {
//         printf("我是一个新进程: tid: 0x%lx, pid: %d, name: %s, cnt: %d\n", 
//                 pthread_self(), getpid(), name.c_str(), cnt);
//         cnt--;
//         sleep(1);
//         // pthread_exit(nullptr);
//     }
//     // 新进程退出
//     pthread_exit(nullptr);
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     char threadname[g_size]; 
//     snprintf(threadname, g_size, "thread-%d", 1);
//     pthread_create(&tid, NULL, threadrun, (void*)threadname);

//     // pthread_exit(nullptr);
//     while(true)
//         pause();

    
//     return 0;
// }

// 3. pthread_cancel() 取消线程 -- 里面用到了线程等待 -- 一般不太推荐第三种
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>

// const int g_size = 64;
// void* threadrun(void* arg)
// {
//     std::string name = static_cast<const char*>(arg);
//     int cnt = 5;
//     while(cnt)
//     {
//         printf("我是一个新进程: tid: 0x%lx, pid: %d, name: %s, cnt: %d\n", 
//                 pthread_self(), getpid(), name.c_str(), cnt);
//         cnt--;
//         sleep(1);
//         // pthread_cancel(pthread_self()); // 自己取消自己
//     }
//     return (void*)10;
// }

// int main()
// {
//     pthread_t tid;
//     char threadname[g_size]; 
//     snprintf(threadname, g_size, "thread-%d", 1);
//     pthread_create(&tid, NULL, threadrun, (void*)threadname);

//     int n = pthread_cancel(tid);
//     if(n != 0)
//     {
//         printf("pthread_cancel error: %d\n", n);
//     }
//     else
//     {
//         printf("pthread_cancel success\n");
//     }
    
//     void* ret = nullptr;
//     pthread_join(tid, &ret);
//     printf("join 0x%lx success, ret code: %lld\n", tid, (long long)ret);
//     while(true)
//         pause();

    
//     return 0;
// }


// // 测试线程等待
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <string>

// const int g_size = 64;
// void* threadrun(void* arg)
// {
//     std::string name = static_cast<const char*>(arg);
//     int cnt = 5;
//     while(cnt)
//     {
//         printf("我是一个新进程: tid: 0x%lx, pid: %d, name: %s, cnt: %d\n", 
//                 pthread_self(), getpid(), name.c_str(), cnt);
//         cnt--;
//         sleep(1);
//     }
//     // return (void*)10;
//     pthread_exit((void*)100); // 再试试这个
// }

// int main()
// {
//     pthread_t tid;
//     char threadname[g_size]; 
//     snprintf(threadname, g_size, "thread-%d", 1);
//     pthread_create(&tid, NULL, threadrun, (void*)threadname);
    
//     void* ret = nullptr;
//     pthread_join(tid, &ret);
//     printf("join 0x%lx success, ret code: %lld\n", tid, (long long)ret);
//     while(true)
//         pause();

    
//     return 0;
// }

// 多线程的优化
#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include "Task.hpp"

int g_size = 64;
void* threadrun(void* arg)
{
    Task *t = static_cast<Task*>(arg);
    t->Execute();
    return t;
}

// ./CreateThread threadnum;
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s threadnum\n", argv[0]);
        return -1;
    }
    int threadnum = std::stoi(argv[1]);
    std::vector<pthread_t> tids;
    for(int i = 0; i < threadnum; i++)
    {
        pthread_t tid;
        char threadname[g_size]; // 这个是不太行的, 如果要让每个线程都有自己的名称, 则需要动态分配内存, 不能使用栈内存
        // char* threadname = (char*)malloc(g_size);
        snprintf(threadname, g_size, "thread-%d", i + 1);

        Task *t = new Task(threadname, 10 + i, 20 * i);
        pthread_create(&tid, NULL, threadrun, (void*)t);
        tids.push_back(tid);
        std::cout << "create thread " << threadname << " success, tid: " << tid << std::endl;
        // sleep(1);
    }


    // 等待所有线程执行完成
    std::vector<Task*> result_list;
    for(auto& tid : tids)
    {
        Task *t = nullptr;
        pthread_join(tid, (void**)&t);
        result_list.push_back(t);
        std::cout << "join success: " << tid << std::endl;
    }

    // 处理结果清单
    std::cout << "result list: " << std::endl;
    for(auto& t : result_list)
    {
        std::cout << t->GetResult() << std::endl;
    }

    return 0;
}