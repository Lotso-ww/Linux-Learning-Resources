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



namespace LogMoudle
{
    // 1. 获取时间
    std::string GetTimeStamp()
    {
        time_t currentTime = time(nullptr); // 默认获取当前时区的时间
        // 我们希望把这个时间转换成年-月-日 时:分:秒
        struct tm dataTime;
        localtime_r(&currentTime, &dataTime);

        char dataTimeStr[128];
        snprintf(dataTimeStr, sizeof(dataTimeStr), "%4d-%02d-%02d %02d:%02d:%02d", 
                 dataTime.tm_year + 1900,
                 dataTime.tm_mon + 1,
                 dataTime.tm_mday,
                 dataTime.tm_hour,
                 dataTime.tm_min,
                 dataTime.tm_sec
                );
        
        return dataTimeStr;
    }

    // 2. 日志等级 -- 枚举类型(整数)转换成字符串类型
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
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
    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default; // 不在这里析构
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
        FileLogStrategy(const std::string &logdir = gdefaultlogdir, const std::string &logfilename = gdefaultlogfilename)
            :_logdir(logdir),
            _logfilename(logfilename)
        {
            LockGuard lockGuard(&_mutex);
            if(std::filesystem::exists(_logdir))
            {
                return;
            }
            else 
            {
                try 
                {
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
            LockGuard logGuard(&_mutex);
            std::string target = _logdir + _logfilename;
            std::ofstream out(target, std::ios::app); // 追加
            if(!out.is_open()) // 打开文件
            {
                return;
            }
            out << message << "\n"; // 流式写入
            out.close();
        }
    private:
        std::string _logdir;
        std::string _logfilename;
        Mutex _mutex;
    };

    class Logger 
    {
    public:
        Logger()
        {
            UseConsoleLogStrategy();
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
    
        // 内部类
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, const std::string &filename, int line, Logger &self)
                : _currenttime(GetTimeStamp())
                , _loglevel(LogLevel2String(level))
                , _pid(getpid())
                , _filename(filename)
                , _line(line)
                , _logger(self)
            {
                std::stringstream ss;
                ss << "[" << _currenttime << "] "
                   << "[" << _loglevel << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "] "
                   << "- ";
                
                _loginfo = ss.str(); // 直接拼上去
            }
            ~LogMessage()
            {
                if(_logger._strategy)
                {
                    // 走到尽头了,调用刷新策略刷新出来
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

            // 用模版
            template <typename T>
            LogMessage& operator << (const T& info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }  
        private:
            std::string _currenttime;
            std::string _loglevel;
            int _pid;
            std::string _filename;
            int _line;
            std::string _loginfo;

            Logger &_logger; // 外部类引用
        };

        // Logger对象打印日志的时候，故意返回一个LogMessage的临时对象
        LogMessage operator() (LogLevel level, const std::string filename, int line)
        {
            return LogMessage(level, filename, line, *this);
        }
    private:
        std::unique_ptr<LogStrategy> _strategy; // 策略
    };

    // 定义一个全局模块的Logger对象, 方便后续的使用
    Logger logger;

// 定义宏
#define LOG(level) logger(level, __FILE__, __LINE__)

// 还是使用宏: 开启控制台的日志策略
#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleLogStrategy()
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileLogStrategy()
}
#endif