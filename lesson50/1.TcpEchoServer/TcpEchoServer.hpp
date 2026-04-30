#ifndef __TCP__ECHOSERVER__HPP
#define __TCP__ECHOSERVER__HPP

#include <csignal>
#include <cstdint>
#include <pthread.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "InetAddr.hpp"
#include "Logger.hpp"
using namespace LogModule;

static const uint16_t gdefaultport = 8080;
static const int gbacklog = 32;

class TcpEchoServer 
{
private:
    // sockfd: 既可以支持读，又可以支持写, TCP socket也是全双工的.
    void Service(int sockfd, InetAddr client)
    {
        // 长连接服务
        while (true) 
        {
            char inbuffer[1024];

            // 1. 读取
            int n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            if(n > 0)
            {
                inbuffer[n] = 0;
                LOG(LogLevel::INFO) << client.StringAddress() << " say# " << inbuffer;
            }
            else if(n == 0)
            {
                LOG(LogLevel::INFO) << client.StringAddress() << " close sockfd: " << sockfd << ", me too!";
                break;
            }
            else 
            {
                LOG(LogLevel::ERROR) << "read socket error";
                break;
            }

            // 加工处理数据
            std::string echo_string = "server echo# ";
            echo_string += inbuffer;

            // 2. 写回数据
            int m = write(sockfd, echo_string.c_str(),  echo_string.size());
            if(m < 0)
            {
                LOG(LogLevel::ERROR) << "write socket error";
                break;
            }
        }

        close(sockfd);
    }
public:
    TcpEchoServer(uint16_t port = gdefaultport): _port(port), _listensockfd(-1)
    {}
    void Init()
    {
        // 1. 创建套接字
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0); // 设置成0就可以了
        if(_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error: " << _listensockfd;
            exit(2);
        }
        LOG(LogLevel::INFO) << "create socket success: " << _listensockfd;

        // 2.bind
        // 我们这里是可以直接使用我们的InetAddr的,但是我们后面再用
        struct sockaddr_in local;
        socklen_t len = sizeof(local);
        memset(&local, 0, len);
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = htonl(INADDR_ANY);

        int n = bind(_listensockfd, (struct sockaddr*)&local, len);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error: " << _listensockfd;
            exit(3);
        }
        LOG(LogLevel::INFO) << "bind success: " << _listensockfd;

        // 3. 设置成监听
        n = listen(_listensockfd, gbacklog);
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error: " << _listensockfd;
            exit(3);
        }
        LOG(LogLevel::INFO) << "listen success: " << _listensockfd;
    }
    class ThreadData
    {
    public:
        ThreadData(int sockfd, InetAddr addr, TcpEchoServer *owner)
                : _sockfd(sockfd)
                , _address(addr)
                , _owner(owner)       
        {}
        ~ThreadData(){}
    public:
        int _sockfd;
        InetAddr _address;
        TcpEchoServer *_owner;
    };
    // 静态的
    static void* threadRun(void* args)
    {
        // 分离
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData*>(args);
        td->_owner->Service(td->_sockfd, td->_address);
        delete td;
        return nullptr;
    }
    void Start()
    {
        // 多进程版本等待的最佳实践
        // signal(SIGCHLD, SIG_IGN);
        while(true)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&clientaddr, &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                continue;
            }
            // 网络转主机
            InetAddr clientaddress(clientaddr);
            LOG(LogLevel::INFO) << "get a new link: " << clientaddress.StringAddress() << " sockfd: " << sockfd;
            
        
            // 处理连接, 进行IO通信
            // version2 -- 多线程版本
            pthread_t tid;
            ThreadData* td = new ThreadData(sockfd, clientaddress, this);
            pthread_create(&tid, nullptr, threadRun, (void*)td);
            // 我们这里不要去等待, 而是在回调的函数里面使用线程分离

            // // version1 -- 多进程版本
            // pid_t pid = fork();
            // if(pid < 0)
            // {
            //     LOG(LogLevel::ERROR) << "fork error";
            //     close(sockfd);
            // }
            // else if(pid == 0)
            // {
            //     // 子进程,拷贝父进程的文件描述符表，从而和父进程看到同一批文件
            //     // 关闭自己不需要的文件fd
            //     close(_listensockfd);
            //     // **************************************
            //     // 子进程
            //     // if(fork() > 0)
            //     //     exit(0);
            //     // // 孙子进程 -- 你去执行
            //     // Service(sockfd, clientaddress);
            //     // **************************************
            //     Service(sockfd, clientaddress);
            //     exit(0);
            // }
            // else{}
            // // 父进程不需要这个
            // close(sockfd);
            // // 父进程
            // pid_t rid = waitpid(pid, nullptr, 0);

            // // version 0 -- 一般根本不会使用这种
            // Service(sockfd, clientaddress);
        }
    }
    ~TcpEchoServer(){}
private:
    uint16_t _port;
    int _listensockfd;
};
#endif