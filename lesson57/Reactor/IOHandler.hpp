#ifndef __IOHANDLER__HPP
#define __IOHANDLER__HPP

#include "Connection.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include <cerrno>
#include <functional>
#include <sys/socket.h>
static const int gbuffersize = 1024;

using OnMessage_t = std::function<std::string(std::string &inbuffer, int *code)>;

class IOHandler : public Connection
{
public:
    IOHandler(int sockfd, OnMessage_t on_Message)
        :_sockfd(sockfd)
        ,_on_Message(on_Message)
    {}
    int Sockfd() override
    {
        return _sockfd;
    }
    void Close() override
    {
        close(_sockfd);
    }
    void Recver() override
    {
        LOG(LogLevel::INFO) <<"IOHandler Event Readdy, sockfd is: " << _sockfd;
        // 注意, 我们之前设置了ET模式, 所以这里我们也是需要循环式的去读取的
        char buffer[gbuffersize];
        while(true)
        {
            int n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            if(n > 0)
            {
                buffer[n] = 0;
                _inbuffer += buffer; // 加到输入缓冲区后面去
            }
            else if(n == 0)
            {
                // 对端关闭, 我们这里不需要管, 交给异常处理
                LOG(LogLevel::INFO) << "clien quit, address is: " << _clientaddr.StringAddress() << " sockfd is: " << _sockfd;
                Excepter();
                return; // 这需要注意, 一定是用return而不是break
            }
            else 
            {
                // 走到这里, 不一定是错误, 我们需要判断一下
                if(errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if(errno == EINTR)
                {
                    continue;
                }
                else  
                {
                    // 这就是错误了
                    LOG(LogLevel::ERROR) << "recv error, address is: " << _clientaddr.StringAddress() << " sockfd is: " << _sockfd;
                    Excepter();
                    return;
                }
            }
        }
        // 到了这里我们就需要进行一下判断报文完整性了(后续需要回复了), 这个工作不应该我们来做, 交给协议
        // 我们就需要什么呢? 回调函数
        int code = 0;
        std::string result = _on_Message(_inbuffer, &code);
        if(code == 0)
        {
            _outbuffer += result;
        }
        else  
        {
            Excepter();
            return;
        }

        // version1 -- 不为空直接发送
        if(!_outbuffer.empty())
                Sender();
        // // Version2 -- 也可以直接使能
        // if(!_outbuffer.empty())
        //     _R->EnableReadWrite(_sockfd, true, true);
    }
    void Sender() override
    {
        while(true)
        {
            int n = send(_sockfd, _outbuffer.c_str(), _outbuffer.size(), 0);
            if(n >= 0)
            {
                _outbuffer.erase(0, n);
                if(_outbuffer.empty())
                    break;
            }
            else  
            {
                // 不一定是错误, 判断
                if(errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }    
                else if(errno == EINTR)
                {
                    continue;
                }
                else  
                {
                    // 这里就是真的错误了
                    LOG(LogLevel::ERROR) << "send error, address is: " << _clientaddr.StringAddress() << " sockfd: " << _sockfd;
                    Excepter(); // 交给异常处理
                    return;
                }
            }
        }

        // 走到这里, 两种情况
        // 1. 发送完了
        // 2. 还没发完, 写条件不满足, 这里就有说法了
        if(_outbuffer.empty()) // 为空了
            _R->EnableReadWrite(_sockfd, true, false);
        else // 对conn，修改sockfd关心的事件，epoll ->EPOLLOUT
            _R->EnableReadWrite(_sockfd, true, true); // 写也可以设置为true了

    }
    void Excepter() override
    {
        LOG(LogLevel::ERROR) << "Excepter, address is: " << _clientaddr.StringAddress() << " sockfd: " << _sockfd;
        // 实现异常处理其实很简单, 其实就是把这个文件描述符(连接)清除掉就行
        _R->DelConnection(_sockfd);
    }
    ~IOHandler(){}
private:
    int _sockfd;
    OnMessage_t _on_Message;
    std::string _inbuffer;  // 输入缓冲区
    std::string _outbuffer; // 输出缓冲区
};

#endif