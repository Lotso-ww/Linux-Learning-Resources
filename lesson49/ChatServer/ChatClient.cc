// 客户端我们就不封装了,也不使用日志了
#include "InetAddr.hpp"
#include "Thread.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

// 初始化为全局的方便在函数中去使用
int sockfd = -1;
std::string server_ip;
uint16_t server_port = 0;

void Usage(const std::string &name)
{
    std::cerr << "Usage: " << name << " server_ip server_port" << std::endl;
}
void InitClient()
{
    // 1. 创建 sockefd
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "create client socketfd error" << std::endl;
        exit(1);
    }
}
void sendMessage()
{
    // 2. 构建目标服务器socket信息
    InetAddr server(server_port, server_ip);

    // 3. 发送数据和读取数据
    std::string inbuffer;
    while(true)
    {
        std::cout << "Please Enter# ";
        std::getline(std::cin, inbuffer);

        // (1). 发送数据
        ssize_t n = sendto(sockfd, inbuffer.c_str(), inbuffer.size(), 0, server.Addr(), server.AddrLen());
        (void)n;
    }
}
void RecvMessage()
{
    while(true)
    {
        // (2). 接收数据
        struct sockaddr_in temp;
        socklen_t tempLen = sizeof(temp);
        char buffer[1024];
        ssize_t m = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&temp, &tempLen);
        if(m > 0)
            buffer[m] = 0;
        std::cerr << buffer << std::endl; // 2 打印, 往标准错误打
    }
}
// ./UdpEchoClient 1900.0.0.1 8080
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    // 得到我们的服务端IP和Port
    server_ip = argv[1];
    server_port = std::stoi(argv[2]);
    InitClient(); // 初始化

    // 创建两个线程
    Thread sender(sendMessage);
    Thread recver(RecvMessage);

    sender.start();
    recver.start();

    sender.join();
    recver.join();
}