#include <memory>
#include "Dictionary.hpp"
#include "UdpServer.hpp"

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

    ENABLE_CONSOLE_LOG_STRATEGY();

    // std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[1]);

    // 1. 创建一个在线字典服务
    std::unique_ptr<Dictionary> dict = std::make_unique<Dictionary>();

    // 2. 创建一个网络服务器
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>([&dict](const std::string &word)->std::string{
        return dict->TransTrate(word);
    }, server_port);

    // 3. 初始化和启动服务器
    usvr->Init();
    usvr->Start();
}