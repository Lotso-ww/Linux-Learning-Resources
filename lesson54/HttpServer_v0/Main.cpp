#include "HttpServer.hpp"
#include <memory>


void Usage(std::string procname)
{
    std::cout << "Usage: " << procname << " ServerPort" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);

    // 我们现在这里不用lambda了,在HttpServer中实现了
    std::unique_ptr<HttpServer> hsvr = std::make_unique<HttpServer>(port);
    hsvr->Run();

    return 0;
}