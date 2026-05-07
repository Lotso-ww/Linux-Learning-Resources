#include "InetAddr.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


void Usage(std::string procname)
{
    std::cout << "Usage: " << procname << " ServerPort" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string serverIp = argv[1];
    uint16_t serverPort = std::stoi(argv[2]);

    // 1. 创建socket套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
    }

    // 2. 建立连接
    InetAddr serveraddress(serverPort, serverIp);
    int n = connect(sockfd, serveraddress.Addr(), serveraddress.AddrLen());
    if(n < 0)
    {
        std::cerr << "connect error" << std::endl;
        exit(3);
    }

    // 3. sockfd通信过程
    while(true)
    {
        std::string line;
        std::cout << "Please Enter# ";
        std::getline(std::cin, line);

        // 写
        ssize_t n = write(sockfd, line.c_str(), line.size());
        (void)n;

        // 读
        char buffer[4096]; // 开大一点
        ssize_t m = read(sockfd, buffer, sizeof(buffer) - 1);
        if(m > 0)
        {
            buffer[m] = 0;
            std::cout << "-> " << buffer << std::endl;
        }
        else if(m == 0)
        {
            std::cerr << "server quit!" << std::endl;
            break;
        }
        else 
        {
            std::cerr << "read error" << std::endl;
            break;
        }
    }
    close(sockfd);
    return 0;
}