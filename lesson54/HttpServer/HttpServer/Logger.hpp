#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#include <iostream>
#include <cstdio>
#include <string>
#include <ctime>
#include <filesystem> // C++17
#include <fstream>
#include <sstream>
#include <memory>
#include <unistd.h>
#include "Mutex.hpp"

namespace LogModule
{
    // 1. 获取时间
    std::string GetTimeStamp()
    {
        time_t timestamp = time(nullptr);
        struct tm data_time;
        localtime_r(&timestamp, &data_time);

        char data_time_str[128];
        snprintf(data_time_str, sizeof(data_time_str), "%4d-%02d-%02d %02d:%02d:%02d",
                 data_time.tm_year + 1900,
                 data_time.tm_mon + 1,
                 data_time.tm_mday,
                 data_time.tm_hour,
                 data_time.tm_min,
                 data_time.tm_sec);

        return data_time_str;
    }

    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    // 2. 日志等级
    std::string LogLevel2String(LogLevel level)
    {
        switch (level)
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
    // 3. 日志刷新
    // 基类：策略基类, 设置刷新策略的
    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &logmessage) = 0;
    };

    // 子类:继承纯虚接口类
    // 策略1
    class ConsoleLogStrategy : public LogStrategy
    {
    public:
        ConsoleLogStrategy() {}
        ~ConsoleLogStrategy() {}
        void SyncLog(const std::string &logmessage) override
        {
            LockGuard lockguard(&_mutex);
            std::cout << logmessage << std::endl;
        }

    private:
        Mutex _mutex;
    };

    static const std::string glogdir = "/var/log/";
    static const std::string glogfilename = "Online_Cal_Log.txt";
    // 子类:继承纯虚接口类
    // 策略2
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &dir = glogdir, const std::string &filename = glogfilename)
            : _logdir(dir), _logfilename(filename)
        {
            // log / log.txt
            LockGuard lockguard(&_mutex);
            if (std::filesystem::exists(_logdir))
            {
                return;
            }
            else
            {
                try
                {
                    std::filesystem::create_directories(_logdir);
                }
                catch (const std::filesystem::filesystem_error &e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
        ~FileLogStrategy()
        {
        }
        void SyncLog(const std::string &logmessage) override
        {
            LockGuard lockguard(&_mutex);
            std::string target = _logdir + _logfilename;
            std::ofstream out(target, std::ios::app); // 追加写入文件
            if (!out.is_open())
            {
                return;
            }
            out << logmessage << "\n";
            out.close();
        }

    private:
        std::string _logdir;
        std::string _logfilename; // ./log/XXX.log
        Mutex _mutex;
    };

    // 我们真正要的，日志类
    class Logger
    {
    public:
        Logger()
        {
            UseConsoleLogStrategy();
        }
        ~Logger()
        {
        }
        void UseConsoleLogStrategy()
        {
            _strategy = std::make_unique<ConsoleLogStrategy>();
        }
        void UseFileLogStrategy()
        {
            _strategy = std::make_unique<FileLogStrategy>();
        }

        // 内部类: 一条日志
        // 目标是把一个类对象，变成一个string
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string &filename, int line, Logger &self)
                : _level(level),
                  _curr_time(GetTimeStamp()),
                  _pid(getpid()),
                  _filename(filename),
                  _line(line),
                  _logger(self)
            {
                std::stringstream ss;
                ss << "[" << _curr_time << "] "
                   << "[" << LogLevel2String(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "] "
                   << "- ";
                _loginfo = ss.str();
            }

            template <typename T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }

            ~LogMessage() // RAII风格的日志刷新！
            {
                if (_logger._strategy)
                {
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

        private:
            LogLevel _level; //  日志等级

            std::string _curr_time; // 当前时间
            pid_t _pid;             // 进程pid
            std::string _filename;  // 文件名
            int _line;              // 行号
            std::string _loginfo;   // 一条完整的日志

            Logger &_logger; // 外部类的引用
        };

        // Logger 对象打印日志的时候，故意返回一个临时的LogMessage对象
        // 为什么要返回临时内部类对象？？
        LogMessage operator()(LogLevel level, std::string filename, int line)
        {
            return LogMessage(level, filename, line, *this);
        }

    private:
        std::unique_ptr<LogStrategy> _strategy; // 刷新日志的策略
    };

    Logger logger;

// 使用宏，包装我们的日志打印过程，宏有一个特点，#define A B， B替换成为A
#define LOG(level) logger(level, __FILE__, __LINE__)

// 动态调整日志策略
#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleLogStrategy()
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileLogStrategy();
}

#endif