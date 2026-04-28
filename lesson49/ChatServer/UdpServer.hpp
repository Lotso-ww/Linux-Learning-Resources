#ifndef __UDP__ECHOSERVER__HPP
#define __UDP__ECHOSERVER__HPP

#include <cstdint>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <functional>
#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace LogModule;

// 参数就是获得的数据，返回值就是处理完数据的结果
// using callback_t = std::function<std::string(const std::string &)>;
using callback_t = std::function<void (std::string message, InetAddr who, int sockfd)>;

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
        LOG(LogLevel::INFO) << "create socketfd success: " << _socketfd; // 3

        // 2. bind
        InetAddr local(_port);
        int n = bind(_socketfd, local.Addr(), local.AddrLen());
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socketfd error";
        }
        LOG(LogLevel::INFO) << "bind socketfd success: " << _socketfd; // 3
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

            // 我们从peer里面拿到的肯定是网络序列,我们这里需要的是主机序列
            InetAddr clientAddr(perr);
            // LOG(LogLevel::INFO) << "get a message: " << inbuffer
            //                            << ", client addr: " << clientIp << ":" << clientPort;

            // 处理数据
            if(_cb)
            {
               _cb(inbuffer, clientAddr, _socketfd);
            }

            // 我们现在直接在Route里面进行转发的工作
            // // 2. 发送网络数据
            // // 这个len是个输入输出型参数
            // ssize_t m = sendto(_socketfd, result.c_str(), result.size(),  0, (struct sockaddr*)&perr, len);
            // (void)m;
        }
    }

    ~UdpServer()
    {
        if(_socketfd >= 0)
        {
            close(_socketfd);
            _socketfd = -1;
        }
    }
private:
    int _socketfd;
    // std::string _ip; // 可以不需要
    uint16_t _port;

    callback_t _cb;
};
#endif
