#include "UdpEchoServer.hpp"

void Usage(const std::string &name)
{
    std::cerr << "Usage: " << name << " ip port" << std::endl;
}

// ./UdpEchoServer 8080
// 我们不直接绑定固定IP
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    // std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[1]);

    // 初始化和启动
    UdpEchoServer usvr(server_port);
    usvr.Init();
    usvr.Start();
}