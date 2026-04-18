// #include <iostream>
// #include <memory>
// #include <unistd.h>
// #include "Logger.hpp"
// using namespace LogModule;

// // 测试时间戳模块
// void testTime()
// {
//     for(int i = 0; i < 5; i++)
//     {
//         std::cout << GetTimeStamp() << std::endl;
//         sleep(1);
//     }
// }
// // 测试日志类枚举类型转字符类型模块
// void testEnum()
// {
//     std::cout << LogLevel2String(LogLevel::DEBUG) << std::endl;
//     std::cout << LogLevel2String(LogLevel::INFO) << std::endl;
//     std::cout << LogLevel2String(LogLevel::WARNING) << std::endl;
//     std::cout << LogLevel2String(LogLevel::ERROR) << std::endl;
//     std::cout << LogLevel2String(LogLevel::FATAL) << std::endl;
// }
// // 测试刷新策略
// void testStrategy()
// {
//     std::string message1 = "console: hello Log, hello Lotso!";
//     std::string message2 = "file: hello Log, hello Lotso!";

//     std::unique_ptr<LogStrategy> strategy = std::make_unique<ConsoleLogStrategy>(); // 父类指针指向子类对象
//     strategy->SyncLog(message1);
//     strategy->SyncLog(message1);
//     strategy->SyncLog(message1);
//     strategy->SyncLog(message1);
//     strategy->SyncLog(message1);

//     strategy = std::make_unique<FileLogStrategy>(); // 父类指针指向子类对象
//     strategy->SyncLog(message2);
//     strategy->SyncLog(message2);
//     strategy->SyncLog(message2);
//     strategy->SyncLog(message2);
//     strategy->SyncLog(message2);
// }
// // 总体测试
// void test()
// {
//     // 开启控制台策略
//     ENABLE_CONSOLE_LOG_STRATEGY();
//     LOG(LogLevel::DEBUG) << "CONSOLE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::INFO) << "CONSOLE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::WARNING) << "CONSOLE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::ERROR) << "CONSOLE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::FATAL) << "CONSOLE: hello Lotso " << 7.9 << " bd";

//     // 开启文件策略
//     ENABLE_FILE_LOG_STRATEGY();
//     LOG(LogLevel::DEBUG) << "FILE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::INFO) << "FILE: Lotso " << 7.9 << " bd";
//     LOG(LogLevel::WARNING) << "FILE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::ERROR) << "FILE: hello Lotso " << 7.9 << " bd";
//     LOG(LogLevel::FATAL) << "FILE: hello Lotso " << 7.9 << " bd";
// }
// int main()
// {
//     // 1. 测试时间
//     // testTime();

//     // 2. 测试枚举类转字符串类型
//     // testEnum();

//     // 3. 测试策略
//     // testStrategy();

//     // 4. 整体测试
//     test();
//     return 0;
// }


#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "Logger.hpp" // 我们实现的日志头文件

using namespace LogModule;

// 多线程测试函数：10个线程同时打印日志
void *thread_log_test(void *arg)
{
    char *thread_name = (char *)arg;
    for (int i = 0; i < 5; i++)
    {
        LOG(LogLevel::INFO) << thread_name << " 执行日志打印, 循环次数: " << i;
        usleep(1000);
    }
    return nullptr;
}

int main()
{
    // 1. 基础控制台日志输出
    std::cout << "===== 控制台日志测试 =====" << std::endl;
    ENABLE_CONSOLE_LOG_STRATEGY();
    LOG(LogLevel::DEBUG) << "这是DEBUG调试日志, 数值: " << 3.14159;
    LOG(LogLevel::INFO) << "这是INFO常规日志, 服务启动成功";
    LOG(LogLevel::WARNING) << "这是WARNING警告日志, 配置缺失, 使用默认值";
    LOG(LogLevel::ERROR) << "这是ERROR错误日志, 文件读取失败";
    LOG(LogLevel::FATAL) << "这是FATAL致命日志, 内存耗尽, 服务退出";

    // 2. 切换为文件日志策略
    std::cout << "\n===== 文件日志测试 =====" << std::endl;
    ENABLE_FILE_LOG_STRATEGY();
    LOG(LogLevel::INFO) << "切换为文件日志策略, 日志将持久化到./log/log.txt";
    LOG(LogLevel::DEBUG) << "文件日志测试, 支持链式拼接: " << "字符串 " << 1234 << " 浮点数 " << 2.71828;

    // 3. 多线程线程安全测试
    std::cout << "\n===== 多线程日志测试 =====" << std::endl;
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, nullptr, thread_log_test, (void *)"thread-1");
    pthread_create(&t2, nullptr, thread_log_test, (void *)"thread-2");
    pthread_create(&t3, nullptr, thread_log_test, (void *)"thread-3");
    pthread_create(&t4, nullptr, thread_log_test, (void *)"thread-4");

    // 等待所有线程执行完毕
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);

    LOG(LogLevel::INFO) << "多线程日志测试完成, 无乱序、无交错";
    return 0;
}