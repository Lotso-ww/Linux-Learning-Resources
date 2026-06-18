#ifndef __SOCKET__HPP
#define __SOCKET__HPP

#include "InetAddr.hpp"
#include "Logger.hpp"
#include "Common.hpp" // 错误码的维护就放在这个文件里面了
#include <asm-generic/socket.h>
#include <cstdint>
#include <memory>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

using namespace LogModule;

// 全局变量：listen 的第二次参数，代表底层全连接队列的最大长度
static const int gbacklog = 16;


// 基类 -- 把socket创建过程，模版化，方法化 -- 模版方法模式
// 这个方法我们可以掌握一下
class Socket 
{
public:
    virtual ~Socket() {}
    
    /* 纯虚函数：强制派生类根据协议族（TCP/UDP/UNIX域）实现具体逻辑 */
    virtual void CreateSocketOrDie() = 0;
    virtual void BindSocketOrDie(uint16_t port) = 0;
    virtual void ListenSocketOrDie() = 0;
    virtual int Accepter(InetAddr *clientaddress, int *code) = 0;
    virtual void ConnectSocketOrDie(const std::string &serverip, uint16_t serverport) = 0;
    virtual int Socketfd() = 0;
    virtual void Close() = 0;

    // 读取的时候通过输出型参数能带出来
    virtual int Recv(std::string *outstr) = 0;
    virtual int Send(const std::string &outstr) = 0;

public:
    /* 模板方法：定义了服务器启动的算法骨架 */
    void BuildSocketMethod(uint16_t port)
    {
        CreateSocketOrDie();
        BindSocketOrDie(port);
        ListenSocketOrDie();
    }

    /* 模板方法：定义了客户端启动的算法骨架 */
    void BuildClientSocketMethod(const std::string &serverip, uint16_t serverport)
    {
        CreateSocketOrDie();
        ConnectSocketOrDie(serverip, serverport);
    }
};

class TcpSocket : public Socket
{
public:
    TcpSocket(): _sockfd(-1)
    {}
    // 供 Accepter 调用，用现成的文件描述符构造一个 Socket 对象
    TcpSocket(int sockfd): _sockfd(sockfd)
    {}

    void CreateSocketOrDie() override
    {
        // AF_INET: IPv4, SOCK_STREAM: TCP流式传输
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERROR);
        }
        SetNonBlock(_sockfd); // 设置为非阻塞
        int opt = 1;
        // 1. 设置为地址复用
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        // 2. 设置为端口复用
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        LOG(LogLevel::INFO) << "create socket success";
    }

    void BindSocketOrDie(uint16_t port) override
    {
        InetAddr local(port);
        // 将文件描述符与 IP/Port 绑定
        int n = bind(_sockfd, local.Addr(), local.AddrLen());
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error";
            exit(BIND_ERROR);
        }
        LOG(LogLevel::INFO) << "bind socket success";
    }

    void ListenSocketOrDie() override
    {
        // 进入监听状态，开始接收三次握手请求
        int n = listen(_sockfd, gbacklog);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "listen socket error";
            exit(LISTEN_ERROR);
        }
        LOG(LogLevel::INFO) << "listen socket success";
    }

    // 返回值修改成 int
    int Accepter(InetAddr *clientaddress, int *code) override
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        // 从全连接队列中取出一个已经完成三次握手的连接
        int sockfd = accept(_sockfd, CONV(&peer), &len);
        *code = errno;
        if(sockfd < 0)
        {
            return -1;
        }
        *clientaddress = peer; // 我们通过这个把peer带回去
        
        // 返回一个新的 TcpSocket 对象，专门负责与该客户端的 IO 通信
        // 修改: 改成返回一个整形
        return sockfd;
    }

    void ConnectSocketOrDie(const std::string &serverip, uint16_t serverport) override
    {
        InetAddr serveraddress(serverport, serverip); // 本地转网络
        // 这里的sockfd是客户端创建的自己的TcpSockt
        // 客户端主动向服务端发起三次握手
        int n = connect(_sockfd, serveraddress.Addr(), serveraddress.AddrLen());
        if(n != 0)
        {
            LOG(LogLevel::FATAL) << "connect " << serveraddress.StringAddress() << " failed";
            return;
        }
        LOG(LogLevel::INFO) << "connect " << serveraddress.StringAddress() << " success";
    }

    int Socketfd() override
    {
        return _sockfd;
    }

    void Close() override
    {
        if (_sockfd >= 0)
        {
            close(_sockfd);
            _sockfd = -1;
        }
    }

    int Recv(std::string *outstr) override
    {
        char buffer[1024];
        // TCP 是字节流，recv 不保证读到的是完整的逻辑报文
        ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0); // bug
        if(n > 0)
        {
            buffer[n] = 0;
            *outstr += buffer; // +=的本质是拼接，入队列，outstr当做一个字节流队列！
            return n;
        }
        else if(n == 0)
        {
            // 对端关闭了, 返回 0, 我们不在这里处理
            return 0;
        }
        else 
        {
            // 读取异常（如网络中断或被信号打断）
            return -1;
        }
    }

    int Send(const std::string &outstr) override
    {
        // 尝试将数据拷贝到内核发送缓冲区
        return send(_sockfd, outstr.c_str(), outstr.size(), 0);
    }

private:
    int _sockfd; // 核心文件描述符
};

// 我们Udp也是完全可以实现的, 不需要的函数实现为空就行 -- 其实Unix域间的也可以,都能这样使用
// class UdpServer : public Socket
// {
// public:
// private:
// };
#endif
