#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

using func_t = std::function<void()>;

// 这个是有点bug的
static int gunm = 1;

class Thread
{
private:
    void get_pid()
    {
        _pid = getpid();
    }
    void get_lwid()
    {
        _lwid = syscall(SYS_gettid);
    }
    static void* routine(void* args)
    {
        Thread* ts = static_cast<Thread*>(args);
        ts->get_pid();
        ts->get_lwid();
        ts->_func();
        return nullptr;
    }
public:
    Thread(func_t f) : _func(f), _joinable(true)
    {
        _name = "thread-" + std::to_string(gunm++);
    }
    void start()
    {
        int n = pthread_create(&_tid, nullptr, routine, this);
        if(n != 0)
        {
            std::cerr << "pthread_create failed" << std::endl;
        }
    }
    void stop()
    {
        int n = pthread_cancel(_tid);
        if(n != 0)
        {
            std::cerr << "pthread_cancel failed" << std::endl;
        }
    }
    void join()
    {
        if(_joinable)
        {
            int n = pthread_join(_tid, nullptr);
            if(n != 0)
            {
                std::cerr << "pthread_join failed" << std::endl;
            }
            printf("lwp: %d, name: %s, join success\n", _lwid, _name.c_str());
        }
    }
    void detach()
    {
        if(_joinable)
        {
            _joinable = false;
            int n = pthread_detach(_tid);
            if(n != 0)
            {
                std::cerr << "pthread_detach failed" << std::endl;
            }
        }
    }
    ~Thread()
    {}
private:
    pthread_t _tid;
    pid_t _pid;
    pid_t _lwid;
    std::string _name;
    bool _joinable;
    func_t _func;
};

#endif