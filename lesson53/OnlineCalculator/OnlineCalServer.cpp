#include "Logger.hpp"
#include "Protocol.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include "Calculator.hpp"
#include "Daemon.hpp"
#include "TcpServer.hpp"

void Usage(const std:: string procname)
{
    std::cout << "Usage" << procname << " local_port" << std::endl;
}
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    uint16_t port = std::stoi(argv[1]);
    ENABLE_CONSOLE_LOG_STRATEGY();

    // 守护进程这里还没测试
    // ENABLE_FILE_LOG_STRATEGY();
    // Daemon();

    // 业务层 -> OSI 会话层 -> TCP/IP 应用层
    std::unique_ptr<Calculator> cal = std::make_unique<Calculator>();

    // 协议层 —> OSI 表示层 -> TCP/IP 应用层
    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>([&cal](const Request& req){
        return cal->Exec(req);
    });

    // 网络通信层 -> OSI 应用层 -> TCP/IP 应用层
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port, [&protocol](std::string &streamstr){
        return protocol->HandlerRequest(streamstr);
    });
    // 启动服务器
    tsvr->Run();

    return 0;
}