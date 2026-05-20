#ifndef __TCPSERVER__HPP
#define __TCPSERVER__HPP

#include "InetAddr.hpp"
#include "Socket.hpp"
#include <csignal>
#include <functional>
#include <memory>
#include "Logger.hpp"

using namespace LogModule;
using handler_t = std::function<std::string(std::string &)>;

class TcpServer 
{
public:
    TcpServer(int port)
        : _port(port)
        , _listensockfd(std::make_unique<TcpSocket>())
        , _isrunning(false)
    {
        // 使用我们封装好的 -> 把固定的流程封装起来了直接使用模版方法即可
        // 模版方法模式
        _listensockfd->BulidSocketMethod(port);
    }
    void Run(handler_t handler)
    {
        _handlerstream = handler;
        _isrunning = true;
        signal(SIGCHLD, SIG_IGN); // 多进程的等待最佳实践
        while(_isrunning)
        {
            InetAddr clientaddress;
            // 1. clientaddress -- 使用输出型参数可以带出来,不过得在InetAddr里面新实现一个无参构造和赋值运算符重载
            // 2. 得到socket -- 方便后续面向对象化的使用, 这里是返回的对象而不是简单的整形
            auto sockfd = _listensockfd->Accepter(&clientaddress); 
            if(!sockfd)
            {
                LOG(LogLevel::WARNING) << "accepter error";
                continue;
            }
            LOG(LogLevel::DEBUG) << "client addr: " << clientaddress.StringAddress() << " socket: " << sockfd->Socketfd();

            // 我们使用多进程的方式启动
            if(fork() == 0)
            {
                // 关闭自己不需要的fd
                _listensockfd->Close();
                // 子进程
                HandlerIo(sockfd, clientaddress);
                exit(0);
            }
            // 父进程
            // 关闭自己不需要的fd
            sockfd->Close();
        }
    }

    void HandlerIo(std::shared_ptr<Socket> sockfd, InetAddr clientaddress)
    {
      // 设计的是长服务 -- 改成短连接
      std::string inbuffer;
      // 读取数据
      int n = sockfd->Recv(&inbuffer); // 底层是 +=, 这个很重要,
                                       // 这样不完整就只会继续往之前的里面加
      if (n == 0) 
      {
        // 对端关闭,打印消息提示
        LOG(LogLevel::INFO) << "client quit: " << clientaddress.StringAddress()
                            << " sockfd: " << sockfd->Socketfd();
      } 
      else if (n < 0) 
      {
        // 读取出异常, 打印消息提示并且退出
        LOG(LogLevel::WARNING) << "recv error";
      }

      // 在发送数据之前我们不能啥都不管就直接发了, 得保证报文的完整性
      // 1. 分析inbuffer保证报文的完整性
      // 2. a. 完整: 提取，处理 b.不完整：什么都不做
      // 3：这个工作谁做？谁不做？ 协议来做！TcpServer自己不做
      std::string outbuffer;
      if (_handlerstream)
        outbuffer = _handlerstream(inbuffer); // 回调函数会调出去，也会再回来！
      // 发送数据 -- 如果不为空就发送
      if (!outbuffer.empty())
        sockfd->Send(outbuffer); // 其实也是有返回值的
    }
private:
    int _port;
    std::unique_ptr<Socket> _listensockfd;
    bool _isrunning;
    handler_t _handlerstream; 
};
#endif