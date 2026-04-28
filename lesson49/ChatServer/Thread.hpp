#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

// 定义线程执行的任务类型，使用包装器增强灵活性
using func_t = std::function<void()>;

// 线程状态枚举：用于构建简单的状态机，确保护法操作
enum class TSTAYUS
{
    THREAD_NEW,     // 新建状态
    THREAD_RUNNING, // 运行状态
    THREAD_STOPPED, // 停止/退出状态
};

// 这个是有点bug的：全局静态变量在多线程并发创建对象时存在“竞态条件”
// 多个线程可能同时执行 gunm++，导致线程编号重复，生产环境下建议使用 std::atomic<int>
static int gunm = 1;

class Thread
{
private:
    // 获取所属进程的 PID
    void get_pid()
    {
        _pid = getpid();
    }
    // 获取内核级线程 ID (LWP ID)，这才是 Linux 系统监控（如 top -H）看到的真正 ID
    void get_lwid()
    {
        // 原生 pthread 库没有直接获取 LWP 的接口，必须通过系统调用
        _lwid = syscall(SYS_gettid);
    }

    /**
     * @brief 静态成员函数作为线程入口点
     * 关键逻辑：pthread_create 要求回调函数必须是 void* (*)(void*)
     * 类的普通成员函数隐含 this 指针，参数不匹配，故必须设为 static。
     * 通过传入 args (this 指针) 重新找回对象上下文。
     */
    static void* routine(void* args)
    {
        Thread* ts = static_cast<Thread*>(args);
        ts->get_pid();
        ts->get_lwid();
        
        // 为线程设置名字，方便在调试器（如 gdb）中识别
        pthread_setname_np(pthread_self(), ts->Name().c_str());
        
        // 执行用户真正传入的任务
        ts->_func();
        return nullptr;
    }

public:
    // 构造函数：完成任务绑定与命名，此时线程尚未在内核中创建
    Thread(func_t f) : _func(f), _joinable(true), _status(TSTAYUS::THREAD_NEW)
    {
        _name = "Worker-" + std::to_string(gunm++);
    }

    // 启动线程：正式调用底层接口
    void start()
    {
        if(_status == TSTAYUS::THREAD_RUNNING)
        {
            std::cerr << "thread is already running" << std::endl;
            return;
        }
        
        // 传入 this 作为 routine 的参数，实现 C 到 C++ 的跨越
        int n = pthread_create(&_tid, nullptr, routine, this);
        if(n != 0)
        {
            std::cerr << "pthread_create failed" << std::endl;
        }
        _status = TSTAYUS::THREAD_RUNNING;
    }

    // 停止线程：通过发送取消请求
    void stop()
    {
        if(_status == TSTAYUS::THREAD_RUNNING)
        {
            // pthread_cancel 是比较暴力的退出方式，依赖线程内部是否存在取消点
            int n = pthread_cancel(_tid);
            if(n != 0)
            {
                std::cerr << "pthread_cancel failed" << std::endl;
            }
            _status = TSTAYUS::THREAD_STOPPED;
        }
        else 
        {
            std::cerr << "thread status is : THREAD_STOPPED or THREAD_NEW" << std::endl;
            return;
        }
    }

    // 资源回收：阻塞等待线程结束
    void join()
    {
        if(_joinable)
        {
            // 只有处于 joinable 状态的线程才需要被 join，否则会产生资源泄露
            int n = pthread_join(_tid, nullptr);
            if(n != 0)
            {
                std::cerr << "pthread_join failed" << std::endl;
            }
            printf("lwp: %d, name: %s, join success\n", _lwid, _name.c_str());
        }
        else {
            printf("lwp: %d, name: %s, join failed, because thread is detached\n", _lwid, _name.c_str());
        }
    }

    // 线程分离：将线程设置为由系统自动回收
    void detach()
    {
        if(_joinable && _status == TSTAYUS::THREAD_RUNNING)
        {
            _joinable = false;
            // 分离后，该线程退出时会自动释放所有资源，无需 join
            int n = pthread_detach(_tid);
            if(n != 0)
            {
                std::cerr << "pthread_detach failed" << std::endl;
            }
        }
    }

    // 获取线程名称接口
    std::string Name()
    {
        return _name;
    }

    ~Thread()
    {
        // 析构函数中未做强制 join，这是为了给使用者留出控制权
        // 但要注意，如果对象销毁时线程还在跑且未 detach，可能会导致程序崩溃
    }

private:
    pthread_t _tid;      // 线程库层面的 ID (用户层 ID)
    pid_t _pid;          // 所属进程 ID
    pid_t _lwid;         // 轻量级进程 ID (内核层真正的线程 ID)
    std::string _name;   // 线程可读性名称
    func_t _func;        // 线程执行的任务包装器
    bool _joinable;      // 是否允许被等待标记
    TSTAYUS _status;     // 当前线程状态机
};

#endif
