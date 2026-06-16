#ifndef __REACTOR__HPP
#define __REACTOR__HPP

#include "Connection.hpp"
#include "Poller.hpp"
#include <cstdint>
#include <memory>
#include <sys/epoll.h>
#include <unordered_map>

static const int gnum = 128;

class Reactor 
{
public:
    Reactor():_epoll(std::make_unique<Poller>())
    {

    }
    void AddConnection(std::shared_ptr<Connection> &conn)
    {
        // 1. 拿到fd和事件, 写透到内核中
        int sockfd = conn->Sockfd();
        uint32_t events = conn->Events();
        _epoll->AddEvents(sockfd, events);
        // 2. 放到_connections 里面管理起来这个连接
        _connections[sockfd] = conn;
    }
    void DisPatcher()
    {
        // 事件派发
        int timeout = 2000;
        while(true)
        {
            int n = _epoll->WaitEvents(revs, gnum, timeout);
            // 开始进行事件派发
            for(int i = 0; i < n; i++)
            {
                int sockfd = revs[i].data.fd;      // 哪一个fd
                uint32_t events = revs[i].events;  // 哪一个事件
                // 如果是异常或者错误, 我们需要改动一下, 改写成读写
                if((events & EPOLLHUP) || (events & EPOLLERR)) events = EPOLLIN | EPOLLOUT;
                // 如果是读事件就绪 -- 我们这里也不需要判断是listensockfd还是normal fd了, 利用多态的属性
                if((events & EPOLLIN) && IsConnection(sockfd)) _connections[sockfd]->Recver();
                // 如果是写事件就绪
                if((events & EPOLLOUT) && IsConnection(sockfd)) _connections[sockfd]->Sender();
            }
        }
    }
    ~Reactor(){}
private:
    bool IsConnection(int fd)
    {
        return _connections.find(fd) != _connections.end();
    }
private:
    // 1. 需要一个 epoll 模型
    std::unique_ptr<Poller> _epoll;
    // 2. 需要一个哈希表管理组织所有的连接
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
    // 3. 已经就绪的事件清单
   struct epoll_event revs[gnum];
};

#endif