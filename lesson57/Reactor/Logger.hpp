#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include "Mutex.hpp"

namespace LogModule
{
    // 1. 获取时间
    std::string GetTimeStamp()
    {
        time_t currentTime = time(nullptr); // 默认获取当前时区的时间
        // 我们希望把这个时间转换成年-月-日 时:分:秒
        struct tm dataTime;
        
        // 使用线程安全的版本 localtime_r，防止在多线程并发获取时间时
        // 因为共享静态全局变量而导致的时间数据覆盖错乱。
        localtime_r(&currentTime, &dataTime);

        char dataTimeStr[128];
        // 使用 snprintf 保证缓冲区不溢出，%02d 确保时间位宽不足时自动补0（如09秒）
        snprintf(dataTimeStr, sizeof(dataTimeStr), "%4d-%02d-%02d %02d:%02d:%02d", 
                 dataTime.tm_year + 1900, // tm_year 是从1900年开始计算的偏移量
                 dataTime.tm_mon + 1,     // tm_mon 范围是 [0, 11]，需加1修正
                 dataTime.tm_mday,
                 dataTime.tm_hour,
                 dataTime.tm_min,
                 dataTime.tm_sec
                );
        
        return dataTimeStr;
    }

    // 2. 日志等级 -- 枚举类型(整数)转换成字符串类型
    // 使用 enum class 强类型枚举，避免命名污染，提高类型检查的严谨性
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    /**
     * @brief 辅助函数：将枚举常量映射为可读字符串
     * 解决强类型枚举无法直接通过 std::cout 打印的问题
     */
    std::string LogLevel2String(LogLevel level)
    {
        switch(level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    // 3. 刷新策略
    // 基类: 策略模式
    // 设计意图：将“日志消息的生成”与“日志消息的输出去向”解耦，方便后续扩展网络、数据库等输出端
    class LogStrategy
    {
    public:
        // 虚析构函数：确保通过基类指针释放子类对象时，子类的资源（如文件句柄）能被正确释放
        virtual ~LogStrategy() = default; // 不在这里析构
        // 纯虚函数：定义统一的刷新接口规范
        virtual void SyncLog(const std::string &message) = 0; // 强制子类对其进行重写
    };

    // 策略1: 控制台日志策略
    // 子类
    class ConsoleLogStrategy: public LogStrategy
    {
    public:
        ConsoleLogStrategy(){}
        ~ConsoleLogStrategy(){}
        void SyncLog(const std::string &message) override // 检查重写的错误
        {
            // 显示器在多线程下是“临界资源”，加锁防止多线程输出字符交织（Interleaving）
            LockGuard logGuard(&_mutex);
            std::cout << message << std::endl;
        }
    private:
        Mutex _mutex;
    };

    const static std::string gdefaultlogdir = "./log/";
    const static std::string gdefaultlogfilename = "log.txt";

    // 策略2：文件类日志策略
    // 子类
    class FileLogStrategy: public LogStrategy
    {
    public:
        // 构造函数：初始化路径并利用 C++17 库确保目录环境就绪
        FileLogStrategy(const std::string &logdir = gdefaultlogdir, const std::string &logfilename = gdefaultlogfilename)
            :_logdir(logdir),
            _logfilename(logfilename)
        {
            // 创建目录前加锁，防止多线程同时执行判断与创建操作引发的竞态条件
            LockGuard lockGuard(&_mutex);
            if(std::filesystem::exists(_logdir))
            {
                return;
            }
            else 
            {
                try 
                {
                    // 递归创建目录（mkdir -p），若权限不足或磁盘满会抛出异常
                    std::filesystem::create_directories(_logdir);
                } 
                catch (std::filesystem::filesystem_error &e) 
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }
        ~FileLogStrategy(){}

        void SyncLog(const std::string &message) override
        {
            // 文件 I/O 是昂贵的临界资源操作，加锁保证单条日志写入的原子性
            LockGuard logGuard(&_mutex);
            std::string target = _logdir + _logfilename;
            
            // 使用 std::ios::app (append) 追加模式，保证新旧日志共存而不被覆盖
            std::ofstream out(target, std::ios::app); // 追加
            if(!out.is_open()) // 打开文件
            {
                return;
            }
            out << message << "\n"; // 流式写入并换行
            out.close(); // 关闭文件流，触发缓冲区刷新
        }
    private:
        std::string _logdir;
        std::string _logfilename;
        Mutex _mutex;
    };

    /**
     * @brief Logger 类：日志系统的中央控制器
     * 内部嵌套了 LogMessage 类来实现精妙的 RAII 自动刷新机制
     */
    class Logger 
    {
    public:
        Logger()
        {
            UseConsoleLogStrategy(); // 默认策略
        }
        void UseConsoleLogStrategy()
        {
            _strategy = std::make_unique<ConsoleLogStrategy>();
        }
        void UseFileLogStrategy()
        {
            _strategy = std::make_unique<FileLogStrategy>();
        }
        ~Logger(){};
    
        // 内部类：负责单条日志的组装和析构刷新
        class LogMessage
        {
        public:
            // 构造函数：预组装日志“前缀”部分
            LogMessage(LogLevel level, const std::string &filename, int line, Logger &self)
                : _currenttime(GetTimeStamp())
                , _loglevel(LogLevel2String(level))
                , _pid(getpid())
                , _filename(filename)
                , _line(line)
                , _logger(self) // 保存引用，以便在析构时找到所属的 Logger 进行刷新
            {
                std::stringstream ss;
                ss << "[" << _currenttime << "] "
                   << "[" << _loglevel << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "] "
                   << "- ";
                
                _loginfo = ss.str(); // 此时前缀已拼入缓冲区
            }

            /**
             * @brief 核心设计：RAII 机制触发刷新
             * 当 LOG(...) << "msg"; 这行语句执行完毕，临时对象生命周期结束，
             * 在析构函数中调用策略接口，保证日志在写完即刻、必然被刷出。
             */
            ~LogMessage()
            {
                if(_logger._strategy)
                {
                    // 走到尽头了,调用刷新策略刷新出来
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

            // 用模版重载 << 运算符：接纳各种类型（int, string, double等）
            template <typename T>
            LogMessage& operator << (const T& info)
            {
                std::stringstream ss;
                ss << info; // 自动完成类型转换
                _loginfo += ss.str(); // 追加到内容主体中
                return *this; // 返回引用支持链式调用，如 LOG << a << b << c;
            }   
        private:
            std::string _currenttime;
            std::string _loglevel;
            int _pid;
            std::string _filename;
            int _line;
            std::string _loginfo;

            Logger &_logger; // 外部类引用：用于访问具体刷新策略
        };

        /**
         * @brief 重载仿函数 operator()
         * 这是桥梁：将宏参数传入，并返回一个持有 Logger 权限的临时消息对象
         */
        LogMessage operator() (LogLevel level, const std::string filename, int line)
        {
            return LogMessage(level, filename, line, *this);
        }
    private:
        // 使用 unique_ptr 配合策略基类实现运行时多态
        std::unique_ptr<LogStrategy> _strategy; // 策略
    };

    // 定义一个全局模块的Logger对象, 方便后续的使用
    Logger logger;

// 定义宏：捕获编译器内置变量 __FILE__ 和 __LINE__，简化用户调用 API
#define LOG(level) logger(level, __FILE__, __LINE__)

// 便捷切换输出目的地的宏定义
#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleLogStrategy()
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileLogStrategy()
}
#endif
