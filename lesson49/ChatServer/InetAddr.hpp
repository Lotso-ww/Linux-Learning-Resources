#ifndef __INETADDR__HPP
#define __INETADDR__HPP

#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CONV(addr) (struct sockaddr*)(addr)

class InetAddr 
{
public:
    // 网络转本地
    InetAddr(struct sockaddr_in &addr): _net_addr(addr)
    {
        _port = ntohs(_net_addr.sin_port);
        _ip = inet_ntoa(_net_addr.sin_addr);
    }
    // 本地转网络
    // 可以给引用
    // 这里缺省,我们服务端可以直接传个端口就行,允许任意IP地址, INADDR_ANY
    // 客户端的话我们使用的时候需要指定对应的服务端地址 -- ?
    InetAddr(uint16_t port, const std::string ip = "0.0.0.0")
        : _port(port)
        , _ip(ip)
    {
        _net_addr.sin_family = AF_INET;
        _net_addr.sin_port = htons(_port);
        _net_addr.sin_addr.s_addr = inet_addr(_ip.c_str()); 
    }
    uint16_t Port() const { return _port; }
    std::string Ip() const { return _ip; }
    struct sockaddr *Addr()
    {
        return CONV(&_net_addr);
    }
    socklen_t AddrLen()
    {
        return sizeof(_net_addr);
    }
    bool operator==(const InetAddr &who) const
    {
        return (_ip == who._ip) && (_port == who._port);
    }
    std::string StringAddress() const
    {
        return "[" + _ip + ":" + std::to_string(_port) + "]";
    }
    ~InetAddr()
    {}
private:
    // 本地地址
    uint16_t _port;
    std::string _ip;
    // 网络地址
    struct sockaddr_in _net_addr;
};
#endif