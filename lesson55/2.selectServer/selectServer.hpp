#ifndef __SELECTSERVER__HPP
#define __SELECTSERVER__HPP

#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdint>
#include <memory>
#include <sys/select.h>

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
    void Start()
    {
        fd_set rfds; // read fd set
        while(true)
        {
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
                HandlerEvent();
                break;
            }
        }
    }
    ~selectServer()
    {}
private:
    void HandlerEvent()
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
            if(pos == NUM)
            {
                // 遍历完了找不到
                close(fd);
            }
            else
            {
                // 把这个新获取到的加进辅助数组即可
                // 下一轮循环的时候, 会关心上的
                arr_fds[pos] = fd;
            }
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listenfd;
    // 需要一个辅助数组
    int arr_fds[NUM];
};
#endif