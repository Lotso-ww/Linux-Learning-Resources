#include <functional>
#include <memory>
#include "Route.hpp"
#include "UdpServer.hpp"
#include "ThreadPool.hpp"

void Usage(const std::string &name)
{
    std::cerr << "Usage: " << name << " port" << std::endl;
}

using task_t = std::function<void()>; // 定义一个任务

// ./ChatServer 8080
// 我们不直接绑定固定IP
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();

    // std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[1]);
    std::unique_ptr<Route> r = std::make_unique<Route>();

    // 方法1: 利用两个lambda表达式 -- 最佳实践
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(
        [&r](std::string message, InetAddr who, int sockfd){
            LOG(LogLevel::INFO) << "1. get a message: " << message
                                    << ", client addr: " << who.Ip() << ":" << who.Port();

            // 我们把包装出一个任务,然后入线程池 -- 这个地方有点复杂
            auto task = [&r, &message, &who, &sockfd]()
            {
                r->RouteMessage(message, who, sockfd);
            };
            ThreadPool<task_t>::GetInstance()->Enqueue(task);
    }, server_port);
    usvr->Init();
    usvr->Start();

    // 方法2: 利用std::bind
    // std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(
    //     [&r](std::string message, InetAddr who, int sockfd){
    //         task_t task = std::bind(&Route::RouteMessage, r.get(), message, who, sockfd);
    //         ThreadPool<task_t>::GetInstance()->Enqueue(task);
    // }, server_port);

    return 0;
}