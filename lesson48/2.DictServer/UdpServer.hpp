#ifndef __UDP__SERVER__HPP
#define __UDP__SERVER__HPP

#include <cstdint>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <functional>
#include "logger.hpp"

using namespace LogModule;

// 参数就是获得的数据，返回值就是处理完数据的结果
using callback_t = std::function<std::string(const std::string &)>;

class UdpServer{
public:
    UdpServer(callback_t cb, uint16_t port)
        : _socketfd(-1)
        , _port(port)
        , _cb(cb)
    {}

    void Init()
    {
        // 1. 创建socket, 系统概念
        _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_socketfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socketfd error";
        }
        LOG(LogLevel::INFO) << "create socketfd success: " << _socketfd;

        // 2. bind
        struct sockaddr_in local;
        socklen_t len = sizeof(local);
        bzero(&local, len);
        local.sin_family = AF_INET;
        local.sin_port = htons(_port); // 这里需要本机转网络
        // 如果server 显示的bind了一个具体IP地址，那么它一般就只能收到发给这个IP地址的报文
        // local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 点分十进制 -> 4字节IP,网络序列的
        local.sin_addr.s_addr = INADDR_ANY; // 任意IP地址

        int n = bind(_socketfd, (struct sockaddr*)&local, len);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socketfd error";
        }
        LOG(LogLevel::INFO) << "bind socketfd success: " << _socketfd;
    }

    void Start()
    {
        char inbuffer[1024];
        while(true)
        {
            struct sockaddr_in perr;
            socklen_t len = sizeof(perr);
            // 1. 读取网络数据
            ssize_t n = recvfrom(_socketfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&perr, &len);
            if(n < 0)
            {
                LOG(LogLevel::WARNING) << "recvfrom error";
                break;
            }
            inbuffer[n] = 0;

            // 我们从peer里面拿到的肯定是网络序列,我们这里打印观察需要的是主机序列
            std::string clientIp = inet_ntoa(perr.sin_addr);
            uint16_t clientPort = ntohs(perr.sin_port);
            LOG(LogLevel::INFO) << "get a message: " << inbuffer
                                       << ", client addr: " << clientIp << ":" << clientPort;

            // 处理数据
            std::string result;
            if(_cb)
            {
                result = _cb(inbuffer);
            }

            // 2. 发送网络数据
            // 这个len是个输入输出型参数
            ssize_t m = sendto(_socketfd, result.c_str(), result.size(),  0, (struct sockaddr*)&perr, len);
            (void)m;
        }
    }

    ~UdpServer() 
    {
      if (_socketfd >= 0) 
      {
        close(_socketfd);
        LOG(LogLevel::INFO) << "socket closed, sockfd: " << _socketfd;
      }
    }
private:
    int _socketfd;
    // std::string _ip; // 可以不需要
    uint16_t _port;

    callback_t _cb;
};
#endif
