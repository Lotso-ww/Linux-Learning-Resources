// 客户端我们就不封装了,也不使用日志了
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

void Usage(const std::string &name)
{
    std::cerr << "Usage: " << name << " server_ip server_port" << std::endl;
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
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    // 1. 创建 sockefd
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "create client socketfd error" << std::endl;
        exit(1);
    }

    // 2. 构建目标服务器socket信息
    // 自己一定需要自己的IP和端口号。
    // 但是，client不能自己显示的bind port，一般客户端都是由OS自己选择IP和Port，
    // 尤其是Port，client的port要让OS随机选择
    // 客户端port，是多少不重要，唯一才重要
    // 服务器port，是多少很重要，唯一是基础
    // client不能自己显示的bind port, 但是必须bind，由OS自己完成，Port随机
    struct sockaddr_in server;
    socklen_t len = sizeof(server);
    // 我们服务端用了bzero,这里就用用memset
    memset(&server, 0, len);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // 3. 发送数据和读取数据
    std::string inbuffer;
    while(true)
    {
        std::cout << "Please Enter# ";
        std::cin >> inbuffer;

        // 1. 发送数据
        ssize_t n = sendto(sockfd, inbuffer.c_str(), inbuffer.size(), 0, (struct sockaddr*)&server, len);
        (void)n;

        // 2. 接收数据
        struct sockaddr_in temp;
        socklen_t tempLen = sizeof(temp);
        char buffer[1024];
        ssize_t m = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&temp, &tempLen);
        if(m > 0)
            buffer[m] = 0;
        std::cout << buffer << std::endl;
    }
}