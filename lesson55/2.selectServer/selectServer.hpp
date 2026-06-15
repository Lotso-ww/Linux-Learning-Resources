#ifndef __SELECTSERVER__HPP
#define __SELECTSERVER__HPP

#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdint>
#include <memory>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define NUM (sizeof(fd_set) * 8)
using namespace LogModule;
const int gdefaultfd = -1;

class selectServer 
{
public:
    selectServer(uint16_t port)
        :_port(port)
        ,_listenfd(std::make_unique<TcpSocket>())
    {
        _listenfd->BulidSocketMethod(_port);
        // 初始化
        for(int i = 0; i < NUM; i++)
        {
            arr_fds[i] = gdefaultfd;
        }
        // 首先把_litenfd 放入数组里面
        arr_fds[0] = _listenfd->Socketfd();
    }
    // 事件派发器
    void Dispatcher()
    {
        fd_set rfds; // read fd set
        while(true)
        {
            PrintFds();
            // rfds 参数重置
            FD_ZERO(&rfds);
            int max_fd = gdefaultfd;
            for(int i = 0; i < NUM; i++)
            {
                if(arr_fds[i] == gdefaultfd)
                    continue;
                // 把新的添加进去
                FD_SET(arr_fds[i], &rfds);
                if(max_fd < arr_fds[i]) max_fd = arr_fds[i]; // 更新最大文件描述符
            }
            // struct timeval timeout = {5,0}; // 我们今天这里先不使用这个
            // int n = select(max_fd + 1, &rfds, nullptr, nullptr, &timeout);
            int n = select(max_fd + 1, &rfds, nullptr, nullptr, nullptr);
            switch (n) 
            {
            case 0:
                LOG(LogLevel::DEBUG) << "time out...";
                break;
            case -1:
                LOG(LogLevel::DEBUG) << "select error...";
                break;
            default:
                LOG(LogLevel::DEBUG) << "事件就绪...: n: " << n;
                EventHandler(rfds);
                break;
            }
        }
    }
    ~selectServer()
    {
        // 不需要 arr_fds[i] = gdefaultfd
        // 因为对象马上就销毁了，数组也没了
        for(int i = 0; i < NUM; i++)
        {
            if(arr_fds[i] != gdefaultfd)
                close(arr_fds[i]);
        }
    }
private:
    void EventHandler(fd_set &rfds)
    {
        for(int i = 0; i < NUM; i++)
        {
            if(arr_fds[i] == gdefaultfd) continue;
            // 走到这里可以判断是合法的, 但是我们还需要判断是否就绪
            if(FD_ISSET(arr_fds[i], &rfds))
            {
                // 走到这里就肯定是就绪了
                // 再判断是普通的还是listen
                if(arr_fds[i] == _listenfd->Socketfd())
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
                if(arr_fds[pos] == gdefaultfd)
                    break;
            }
            if(pos >= NUM)
            {
                // 遍历完了找不到
                LOG(LogLevel::WARNING) << "server is full!";
                close(fd);
            }
            else
            {
                // 把这个新获取到的加进辅助数组即可
                // 下一轮循环的时候, 会关心上的
                arr_fds[pos] = fd;
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
        ssize_t n = recv(arr_fds[i], inbuffer, sizeof(inbuffer) - 1, 0);
        if(n > 0)
        {
            inbuffer[n] = 0;
            std::cout << "client say@ " << inbuffer << std::endl;

            // 需要写回去, 这里我们需要等待嘛
            // 其实是先不需要的,写缓冲区默认有数据
            std::string send_string = "echo# ";
            send_string += inbuffer;
            send(arr_fds[i], send_string.c_str(), send_string.size(), 0);
        }
        else if(n == 0)
        {
            // 对端关闭了连接
            LOG(LogLevel::INFO) << "sockfd is close";
            close(arr_fds[i]); // 关闭文件描述符
            arr_fds[i] = gdefaultfd; // 辅助数组里面也处理一下 
        }
        else
        {
            LOG(LogLevel::ERROR) << "read error";
            close(arr_fds[i]); // 关闭文件描述符
            arr_fds[i] = gdefaultfd; // 辅助数组里面也处理一下 
        }
    }
    void PrintFds()
    {
        std::cout << "Select Server list: ";
        for(int i = 0; i < NUM; i++)
        {
            if(arr_fds[i] == gdefaultfd) continue;
            std::cout << arr_fds[i] << " ";
        }
        std::cout << std::endl;
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listenfd;
    // 需要一个辅助数组
    int arr_fds[NUM];
};
#endif