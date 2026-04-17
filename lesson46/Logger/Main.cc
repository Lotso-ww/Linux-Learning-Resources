#include <iostream>
#include <memory>
#include <unistd.h>
#include "Logger.hpp"
using namespace LogMoudle;

// 测试时间戳模块
void testTime()
{
    for(int i = 0; i < 5; i++)
    {
        std::cout << GetTimeStamp() << std::endl;
        sleep(1);
    }
}
// 测试日志类枚举类型转字符类型模块
void testEnum()
{
    std::cout << LogLevel2String(LogLevel::DEBUG) << std::endl;
    std::cout << LogLevel2String(LogLevel::INFO) << std::endl;
    std::cout << LogLevel2String(LogLevel::WARNING) << std::endl;
    std::cout << LogLevel2String(LogLevel::ERROR) << std::endl;
    std::cout << LogLevel2String(LogLevel::FATAL) << std::endl;
}
// 测试刷新策略
void testStrategy()
{
    std::string message1 = "console: hello Log, hello Lotso!";
    std::string message2 = "file: hello Log, hello Lotso!";

    std::unique_ptr<LogStrategy> strategy = std::make_unique<ConsoleLogStrategy>(); // 父类指针指向子类对象
    strategy->SyncLog(message1);
    strategy->SyncLog(message1);
    strategy->SyncLog(message1);
    strategy->SyncLog(message1);
    strategy->SyncLog(message1);

    strategy = std::make_unique<FileLogStrategy>(); // 父类指针指向子类对象
    strategy->SyncLog(message2);
    strategy->SyncLog(message2);
    strategy->SyncLog(message2);
    strategy->SyncLog(message2);
    strategy->SyncLog(message2);
}
// 总体测试
void test()
{
    // 开启控制台策略
    ENABLE_CONSOLE_LOG_STRATEGY();
    LOG(LogLevel::DEBUG) << "CONSOLE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::INFO) << "CONSOLE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::WARNING) << "CONSOLE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::ERROR) << "CONSOLE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::FATAL) << "CONSOLE: hello Lotso " << 7.9 << " bd";

    // 开启文件策略
    ENABLE_FILE_LOG_STRATEGY();
    LOG(LogLevel::DEBUG) << "FILE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::INFO) << "FILE: Lotso " << 7.9 << " bd";
    LOG(LogLevel::WARNING) << "FILE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::ERROR) << "FILE: hello Lotso " << 7.9 << " bd";
    LOG(LogLevel::FATAL) << "FILE: hello Lotso " << 7.9 << " bd";
}
int main()
{
    // 1. 测试时间
    // testTime();

    // 2. 测试枚举类转字符串类型
    // testEnum();

    // 3. 测试策略
    // testStrategy();

    // 4. 整体测试
    test();
    return 0;
}