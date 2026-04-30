#ifndef __INETADDR__HPP
#define __INETADDR__HPP

#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 宏定义：将具体的 sockaddr_in 指针强制转换为通用的 sockaddr 指针
// 许多套接字系统调用（如 bind, recvfrom）要求传入通用地址结构
#define CONV(addr) (struct sockaddr*)(addr)

class InetAddr 
{
public:
    // 网络转本地：主要用于接收消息后，解析对端的 IP 和 端口
    InetAddr(struct sockaddr_in &addr): _net_addr(addr)
    {
        // ntohs: Network to Host Short，将 16 位网络字节序（大端）转为主机字节序（通常是小端）
        _port = ntohs(_net_addr.sin_port);
        // inet_ntoa: 将 32 位网络数值 IP 转换为点分十进制的字符串形式
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
        // 填充 sockaddr_in 结构体
        _net_addr.sin_family = AF_INET;           // 设置为 IPv4 协议族
        // htons: Host to Network Short，将本地主机字节序转为网络字节序
        _net_addr.sin_port = htons(_port);
        // inet_addr: 将字符串 IP 转换为 32 位网络字节序的数值
        _net_addr.sin_addr.s_addr = inet_addr(_ip.c_str()); 
    }

    // 获取主机字节序的端口号
    uint16_t Port() const { return _port; }
    
    // 获取点分十进制字符串 IP
    std::string Ip() const { return _ip; }

    // 获取指向底层 sockaddr 结构的指针，用于 bind/sendto 等系统调用
    struct sockaddr *Addr()
    {
        return CONV(&_net_addr);
    }

    // 获取底层结构体的大小，用于套接字系统调用时的长度参数
    socklen_t AddrLen()
    {
        return sizeof(_net_addr);
    }

    // 重载判等运算符：通过 IP 和端口唯一标识一个网络端点
    // 常用于在在线用户列表中查找指定客户端
    bool operator==(const InetAddr &who) const
    {
        return (_ip == who._ip) && (_port == who._port);
    }

    // 将地址信息转化为易读的字符串格式，如 [127.0.0.1:8080]
    // 常用于打印日志信息
    std::string StringAddress() const
    {
        return "[" + _ip + ":" + std::to_string(_port) + "]";
    }

    ~InetAddr()
    {}

private:
    // 本地主机格式的地址信息
    uint16_t _port;
    std::string _ip;
    
    // 原始网络格式的地址结构体
    struct sockaddr_in _net_addr;
};

#endif
