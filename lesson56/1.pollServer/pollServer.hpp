#ifndef __POLLSERVER__HPP
#define __POLLSERVER__HPP

#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdint>
#include <memory>
#include <sys/poll.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>

#define NUM (sizeof(fd_set) * 8)
using namespace LogModule;
const int gdefaultfd = -1;

class pollServer 
{
public:
    pollServer(uint16_t port)
        :_port(port)
        ,_listenfd(std::make_unique<TcpSocket>())
    {
        _listenfd->BulidSocketMethod(_port);
        // 初始化
        for(int i = 0; i < NUM; i++)
        {
            _fdevent[i].fd = gdefaultfd;
            _fdevent[i].events = _fdevent[i].revents = 0;
        }
        // 首先把_litenfd 放入数组里面
        _fdevent[0].fd = _listenfd->Socketfd();
        _fdevent[0].events |= POLLIN; // there is a data to read
    }
    // 事件派发器
    void Dispatcher()
    {
        while(true)
        {
            PrintFds();
            int timeout = 2000;
            // 不需要参数重置了
            int n = poll(_fdevent, NUM, timeout);
            switch (n) 
            {
            case 0:
                LOG(LogLevel::DEBUG) << "time out...";
                break;
            case -1:
                LOG(LogLevel::DEBUG) << "poll error...";
                break;
            default:
                LOG(LogLevel::DEBUG) << "事件就绪...: n: " << n;
                EventHandler();
                break;
            }
        }
    }
    ~pollServer()
    {
        // 不需要 arr_fds[i] = gdefaultfd
        // 因为对象马上就销毁了，数组也没了
        for(int i = 0; i < NUM; i++)
        {
            if(_fdevent[i].fd != gdefaultfd)
                close(_fdevent[i].fd);
        }
    }
private:
    void EventHandler()
    {
        for(int i = 0; i < NUM; i++)
        {
            if(_fdevent[i].fd == gdefaultfd) continue;
            // 走到这里可以判断是合法的, 但是我们还需要判断是否就绪
            if(_fdevent[i].revents & POLLIN)
            {
                // 走到这里就肯定是就绪了
                // 再判断是普通的还是listen
                if(_fdevent[i].fd == _listenfd->Socketfd())
                {
                    // listensockfd
                    Acceptor();
                }
                else 
                {
                    // normal sockfd
                    IOHandler(i);
                }
            }
        }
    }
    void Acceptor()
    {
        InetAddr clientaddr;
        int fd = _listenfd->Accepter(&clientaddr);
        LOG(LogLevel::INFO) << "get a new link...";
        // 你得到了一个新的连接，这个连接怎么处理？？
        // recv(fd)?? 等 + 拷贝， 不能！！
        // fd -> 托管给select-> 只有select具有"等"的能力!-> 如何托管？？ -> 只要把fd添加的辅助数组即可！
        if(fd >= 0)
        {
            // 找一个空闲的位置
            int pos = 0;
            for(;pos < NUM; pos++)
            {
                if(_fdevent[pos].fd == gdefaultfd)
                    break;
            }
            // 为了健壮性 >= 
            if(pos >= NUM)
            {
                // Poll这里, 扩容
                LOG(LogLevel::WARNING) << "poll is full!";
                close(fd);
            }
            else
            {
                // 把这个新获取到的加进辅助数组即可
                // 下一轮循环的时候, 会关心上的
                _fdevent[pos].fd = fd;
                _fdevent[pos].events |= POLLIN;
                _fdevent[pos].revents = 0;
            }
        }
        else 
        {
            LOG(LogLevel::ERROR) << "Accept error!"; 
        }
    }
    void IOHandler(int i)
    {
        char inbuffer[1024];
        // 在之前的逻辑中已经select等待过了, 走到这里直接读就行
        ssize_t n = recv(_fdevent[i].fd, inbuffer, sizeof(inbuffer) - 1, 0);
        if(n > 0)
        {
            inbuffer[n] = 0;
            std::cout << "client say@ " << inbuffer << std::endl;

            // 需要写回去, 这里我们需要等待嘛
            // 其实是先不需要的,写缓冲区默认有数据
            std::string send_string = "echo# ";
            send_string += inbuffer;
            send(_fdevent[i].fd, send_string.c_str(), send_string.size(), 0);
        }
        else if(n == 0)
        {
            // 对端关闭了连接
            LOG(LogLevel::INFO) << "sockfd is close";
            close(_fdevent[i].fd); // 关闭文件描述符
            _fdevent[i].fd = gdefaultfd; 
            _fdevent[i].events = _fdevent[i].revents = 0;
        }
        else
        {
            LOG(LogLevel::ERROR) << "read error";
            close(_fdevent[i].fd); // 关闭文件描述符
            _fdevent[i].fd = gdefaultfd; 
            _fdevent[i].events = _fdevent[i].revents = 0;
        }
    }
    void PrintFds()
    {
        std::cout << "Poll Server fds list: ";
        for(int i = 0; i < NUM; i++)
        {
            if(_fdevent[i].fd == gdefaultfd) continue;
            std::cout << _fdevent[i].fd << " ";
        }
        std::cout << std::endl;
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listenfd;
    struct pollfd _fdevent[NUM];
};
#endif