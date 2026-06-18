#ifndef __LISTENER__HPP
#define __LISTENER__HPP

#include "Common.hpp"
#include "Reactor.hpp" // 在这里是可以包含的不会出现循环依赖的问题
#include "Connection.hpp"
#include "IOHandler.hpp"
#include "Socket.hpp"
#include <cerrno>
#include <cstdint>
#include <sys/epoll.h>
#include <memory>

class Listener : public Connection
{
public:
    Listener(uint16_t port, OnMessage_t on_Message)
        :_port(port)
        ,_listensockfd(std::make_unique<TcpSocket>())
        ,_on_Message(on_Message)
    {
        _listensockfd->BuildSocketMethod(_port);
        LOG(LogLevel::INFO) << "listensockfd create success";
    }
    int Sockfd() override
    {
        return _listensockfd->Socketfd();
    }
    void Close() override
    {
        close(_listensockfd->Socketfd());
    }
    void Recver() override
    {
        LOG(LogLevel::INFO) <<"Listener Event Readdy, sockfd is: " << _listensockfd->Socketfd();
        while(true)
        {
            int errcode = 0;
            InetAddr clientaddr;
            int sockfd = _listensockfd->Accepter(&clientaddr, &errcode);
            if(sockfd >= 0)
            {
                // 获取新连接成功了
                LOG(LogLevel::INFO) << "accept success, " << "get a new sockfd: " << sockfd << " client address: " << clientaddr.StringAddress();
                // 1. 我们需要先把这个sockfd设置为非阻塞, 再进行构建, 包装成connection
                SetNonBlock(sockfd);
                std::shared_ptr<Connection> conn = std::make_shared<IOHandler>(sockfd, _on_Message);
                conn->SetEvents(EPOLLIN | EPOLLET);         // 设置关心的事件
                conn->SetClientAddress(clientaddr);  // 设置对应的客户端地址
                // 2. 我们需要写透到内核, epoll -> Reactor -> AddConnection
                _R->AddConnection(conn);
            }
            else  
            {
                // 走到这里也不一定就是失败了, 我们需要根据错误码进行一下判断
                if(errcode == EAGAIN || errcode == EWOULDBLOCK)
                {
                    LOG(LogLevel::INFO) << "accept finish!";
                    break;
                }
                else if(errcode == EINTR)
                {
                    LOG(LogLevel::INFO) << "accept interupt!";
                    continue;
                }
                else
                {
                    LOG(LogLevel::ERROR) << "accept error";
                    break;
                }
            }
        }
    }
    void Sender() override
    {
        // 忽略
    }
    void Excepter() override
    {
        // 忽略
    }
    ~Listener(){}
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensockfd;
    OnMessage_t _on_Message;
};

#endif