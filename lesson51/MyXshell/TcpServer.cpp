#include "TcpServer.hpp"
#include "ExcuteCommand.hpp"
#include "Logger.hpp"
#include <cstdint>
#include <memory>

void Usage(std::string procname)
{
    std::cout << "Usage: " << procname << " ServerPort" << std::endl;
}
// ./tcp_echo_server 8080
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t ServerPort = std::stoi(argv[1]);
    

    // 1. 创建一个命令行处理的模块
    std::unique_ptr<ExcuteCommand> excute = std::make_unique<ExcuteCommand>();

    // 2. 创建一个网络服务模块
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(ServerPort);

    tsvr->Init([&excute](std::string cmdstring)->std::string{
        return excute->Excute(cmdstring);
    });
    tsvr->Start();
    return 0;
}