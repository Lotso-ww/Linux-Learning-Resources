#ifndef __Poller__HPP
#define __Poller__HPP

#include "Common.hpp"
#include "Logger.hpp"
#include <cstdint>
#include <cstdlib>
#include <sys/epoll.h>
using namespace LogModule;
static const int gsize = 128;

class Poller 
{
public:
    Poller()
    {
        _epfd = epoll_create(gsize);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create fail";
            exit(EPOLL_ERROR);
        }
        LOG(LogLevel::INFO) << "epfd create success: " << _epfd;
    }
    void AddEvents(int sockfd, uint32_t events)
    {
        struct epoll_event evn;
        evn.events = events;
        evn.data.fd = sockfd;
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &evn);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_ctl add error";
        }
    }
    int WaitEvents(struct epoll_event revs[], int maxevents, int timeout)
    {
        int n = epoll_wait(_epfd, revs, maxevents, timeout);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_wait error";
        }
        return n;
    }
    ~Poller(){}
private:
    int _epfd;
};

// 下面这种形式也是可以的, 这样就可以分别实现select,poll,epoll版本的了
// 帮我们监听所有的fd是否就绪！！
// class Poller
// {
// public:
//     virtual bool Create() = 0;
//     virtual bool Destroy() = 0;
//     virtual void GetEvents() = 0;
//     virtual void SetFdEvent() = 0;
// };

// class SelectPoller : Poller
// {

// };

// class PollPoller:Poller
// {

// };

// class EpollPoller: Poller
// {

// };
#endif