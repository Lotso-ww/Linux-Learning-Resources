#ifndef __EPOLLSERVER__HPP
#define __EPOLLSERVER__HPP

#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <sys/epoll.h>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace LogModule;
static const int gsize = 128;
static const int gnum = 64;


class epollServer 
{
public:
    epollServer(int port)
        :_port(port)
        ,_listensockfd(std::make_unique<TcpSocket>())
    {
        // 创建监听套接字
        _listensockfd->BulidSocketMethod(_port);
        // 创建epoll模型
        _epfd = epoll_create(gsize);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error";
            exit(1);
        }
        LOG(LogLevel::INFO) << "listen sockfd : " << _listensockfd->Socketfd() << " " << "epfd: " << _epfd;
    }
    void Dispatcher()
    {
        // 先将 listensockfd 加进去
        struct epoll_event evn;
        evn.events = EPOLLIN;
        evn.data.fd = _listensockfd->Socketfd();
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensockfd->Socketfd(), &evn);
        if(n == 0)
        {
            LOG(LogLevel::DEBUG) << "epoll_ctl add : " << _listensockfd->Socketfd() << " success";
        }

        int timeout = 2000;
        struct epoll_event revs[gnum]; // 用来记录结果的
        while(true)
        {
            int n = epoll_wait(_epfd, revs, gnum, timeout);
            if(n == 0)
            {
                LOG(LogLevel::INFO) << "time out...";
            }
            else if(n < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_wait error...";
                break;
            }
            else 
            {
                LOG(LogLevel::DEBUG) << "有事件就绪...";
                EventHandler(revs, n); // 事件处理
            }
        }
    }
    ~epollServer()
    {
        if(_epfd >= 0)
            close(_epfd);
        // _listensockfd 会自动释放
    }
private:
    void EventHandler(struct epoll_event revs[], int ready_num)
    {
        for(int i = 0; i < ready_num; i++)
        {
            uint32_t events = revs[i].events; // 那些事件就绪了
            int fd = revs[i].data.fd;         // 那一个 fd
            // 其实这里是可以不需要判断就绪了的, 但是为了健壮性我们还是判断一下
            if(events & EPOLLIN)
            {
                // 再来判断是 listensockfd 还是 normal sockfd
                if(fd == _listensockfd->Socketfd())
                {
                    // litensockfd
                    Acceptor();
                }
                else  
                {
                    // normal sockfd
                    IOHandler(fd);
                }
            }
            else if(events & EPOLLOUT)
            {
                // TODO -- 大家可以自己扩展
            }
        }
    }
    void Acceptor()
    {
        InetAddr clientaddr;
        int sockfd = _listensockfd->Accepter(&clientaddr); // 获取新连接
        LOG(LogLevel::INFO) << "sockfd is: " << sockfd << " client addr: " << clientaddr.StringAddress();
        if(sockfd >= 0)
        {
            struct epoll_event evn;
            evn.events = EPOLLIN;
            evn.data.fd = sockfd;
            int m = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &evn);
            (void)m;
            LOG(LogLevel::INFO) << "epoll_ctl add event: " << sockfd;
        }
        else
        {
            LOG(LogLevel::ERROR) << "Accept error...";
        }
    }
    void IOHandler(int fd)
    {
        // normal fd -- 直接读就行了
        char inbuffer[1024];
        ssize_t n = recv(fd, inbuffer, sizeof(inbuffer) - 1, 0);
        if(n > 0)
        {
            inbuffer[n] = 0;
            LOG(LogLevel::INFO) << "client say# " << inbuffer;

            // 我们回显回去
            std::string echo_string = "echo# ";
            echo_string += inbuffer;
            send(fd, echo_string.c_str(), echo_string.size(), 0);
        }
        else if(n == 0)
        {
            // 对端关闭了
            LOG(LogLevel::INFO) << "client quit, epoll_ctl del event: " << fd;
            // 要删除fd, 首先这个fd得是合法的, 所以我们close一定是在删除之后执行
            epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
        }
        else  
        {
            LOG(LogLevel::ERROR) << "recv error...";
            // 要删除fd, 首先这个fd得是合法的, 所以我们close一定是在删除之后执行
            epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensockfd;
    // epoll 句柄
    int _epfd;
};
#endif