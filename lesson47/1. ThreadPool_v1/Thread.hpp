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

enum  class TSTAYUS
{
    THREAD_NEW,
    THREAD_RUNNING,
    THREAD_STOPPED,
};
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
        pthread_setname_np(pthread_self(), ts->Name().c_str());
        ts->_func();
        return nullptr;
    }
public:
    Thread(func_t f) : _func(f), _joinable(true), _status(TSTAYUS::THREAD_NEW)
    {
        _name = "Worker-" + std::to_string(gunm++);
    }
    void start()
    {
        if(_status == TSTAYUS::THREAD_RUNNING)
        {
            std::cerr << "thread is already running" << std::endl;
            return;
        }
        int n = pthread_create(&_tid, nullptr, routine, this);
        if(n != 0)
        {
            std::cerr << "pthread_create failed" << std::endl;
        }
        _status = TSTAYUS::THREAD_RUNNING;
    }
    void stop()
    {
        if(_status == TSTAYUS::THREAD_RUNNING)
        {
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
        else {
            printf("lwp: %d, name: %s, join failed, because thread is detached\n", _lwid, _name.c_str());
        }
    }
    void detach()
    {
        if(_joinable && _status == TSTAYUS::THREAD_RUNNING)
        {
            _joinable = false;
            int n = pthread_detach(_tid);
            if(n != 0)
            {
                std::cerr << "pthread_detach failed" << std::endl;
            }
        }
    }
    std::string Name()
    {
        return _name;
    }
    ~Thread()
    {}
private:
    pthread_t _tid;
    pid_t _pid;
    pid_t _lwid;
    std::string _name;
    func_t _func;
    bool _joinable;
    TSTAYUS _status;
};

#endif
