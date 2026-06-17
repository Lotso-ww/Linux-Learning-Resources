#ifndef __REACTOR__HPP
#define __REACTOR__HPP

#include "Connection.hpp"
#include "Logger.hpp"
#include "Poller.hpp"
#include <cstdint>
#include <iostream>
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
        // 3. conn 回指 Reactor, 当前对象
        conn->_R = this;

        LOG(LogLevel::INFO) << "insert " << conn->Sockfd() << " into Reactor";
    }
    void EnableReadWrite(int sockfd, bool isRead, bool isWrite)
    {
         if(!IsConnectionExists(sockfd))
            return;
        // TODO -- 待完善
    }
    void LoopOnce(int timeout) 
    {
        int n = _epoll->WaitEvents(revs, gnum, timeout);
        // 开始进行事件派发
        for (int i = 0; i < n; i++) 
        {
            int sockfd = revs[i].data.fd;     // 哪一个fd
            uint32_t events = revs[i].events; // 哪一个事件
            // 如果是异常或者错误, 我们需要改动一下, 改写成读写
            if ((events & EPOLLHUP) || (events & EPOLLERR))
                events = EPOLLIN | EPOLLOUT;
            // 如果是读事件就绪 -- 我们这里也不需要判断是listensockfd还是normal
            // fd了, 利用多态的属性
            if ((events & EPOLLIN) && IsConnectionExists(sockfd))
                _connections[sockfd]->Recver();
            // 如果是写事件就绪
            if ((events & EPOLLOUT) && IsConnectionExists(sockfd))
                _connections[sockfd]->Sender();
        }
    }
    void DisPatcher()
    {
        // 事件派发
        // int timeout = 2000; // 等待2秒
        int timeout = -1; // 有任务来就处理，没有就在epoll阻塞等待, 方便测试观察
        while(true)
        {
            DebugPrint();
            LoopOnce(timeout);
        }
    }
    ~Reactor(){}
private:
    bool IsConnectionExists(int fd)
    {
        return _connections.find(fd) != _connections.end();
    }
    void DebugPrint()
    {
        std::cout << "Reactor sockfd list: ";
        for(auto& conn: _connections)
        {
            std::cout << conn.second->Sockfd() << " ";
        }
        std::cout << std::endl;
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