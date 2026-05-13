#ifndef __SOCKET__HPP
#define __SOCKET__HPP

#include "InetAddr.hpp"
#include "Logger.hpp"
#include <cstdint>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "Logger.hpp"

using namespace LogModule;

static const int gbacklog = 16;

// 枚举错误退出
enum 
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR
};

// 基类 -- 把socket创建过程，模版化，方法化 -- 模版方法模式
// 这个方法我们可以掌握一下
class Socket 
{
public:
    virtual ~Socket() {}
    virtual void CreateSocketOrDie() = 0;
    virtual void BindSocketOrDie(uint16_t port) = 0;
    virtual void ListenSocketOrDie() = 0;
    virtual std::shared_ptr<Socket> Accepter(InetAddr *clientaddress) = 0;
    virtual void ConnectSocketOrDie(const std::string &serverip, uint16_t serverport) = 0;
    virtual int Socketfd() = 0;
    virtual void Close() = 0;

    // 读取的时候通过输出型参数能带出来
    virtual int Recv(std::string *outstr) = 0;
    virtual int Send(const std::string &outstr) = 0;
public:
    void BulidSocketMethod(uint16_t port)
    {
        CreateSocketOrDie();
        BindSocketOrDie(port);
        ListenSocketOrDie();
    }
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
    TcpSocket(int sockfd): _sockfd(sockfd)
    {}

    void CreateSocketOrDie() override
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERROR);
        }
        LOG(LogLevel::INFO) << "create socket success";
    }
    void BindSocketOrDie(uint16_t port) override
    {
        InetAddr local(port);
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
        int n = listen(_sockfd, gbacklog);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "listen socket error";
            exit(LISTEN_ERROR);
        }
        LOG(LogLevel::INFO) << "listen socket success";
    }
    std::shared_ptr<Socket> Accepter(InetAddr *clientaddress) override
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sockfd = accept(_sockfd, CONV(&peer), &len);
        if(sockfd < 0)
        {
            return nullptr;
        }
        *clientaddress = peer; // 我们通过这个把peer带回去
        return std::make_unique<TcpSocket>(sockfd);
    }
    void ConnectSocketOrDie(const std::string &serverip, uint16_t serverport) override
    {
        InetAddr serveraddress(serverport, serverip); // 本地转网络
        // 这里的sockfd是客户端创建的自己的TcpSockt
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
            // 读取异常
            return -1;
        }
    }
    int Send(const std::string &outstr) override
    {
        return send(_sockfd, outstr.c_str(), outstr.size(), 0);
    }
private:
    int _sockfd;
};

// 我们Udp也是完全可以实现的, 不需要的函数实现为空就行 -- 其实Unix域间的也可以,都能这样使用
// class UdpServer : public Socket
// {
// public:
// private:
// };
#endif