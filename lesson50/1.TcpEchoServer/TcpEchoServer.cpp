#include "TcpEchoServer.hpp"
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
    std::unique_ptr<TcpEchoServer> tsvr = std::make_unique<TcpEchoServer>(ServerPort);

    tsvr->Init();
    tsvr->Start();
    return 0;
}