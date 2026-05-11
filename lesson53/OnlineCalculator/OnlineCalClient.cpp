#include "Protocol.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

void Usage(const std:: string procname)
{
    std::cout << "Usage" << procname << " local_port" << std::endl;
}
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    std::unique_ptr<Socket> sockfd = std::make_unique<TcpSocket>();
    sockfd->BuildClientSocketMethod(serverip, serverport);

    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>();

    std::string inbuffer;
    while(true)
    {
        int cnt = 3;
        std::string sendstring;
        while(cnt--)
        {
            // 0. 构建请求
            Request req;
            std::cout << "Enter x# ";
            std::cin >> req._x;
            std::cout << "Enter y# ";
            std::cin >> req._y;
            std::cout << "Enter oper# ";
            std::cin >> req._oper;

            // 1. 序列化(请求)
            std::string reqjsonstr;
            req.Serialize(&reqjsonstr);

            // 2. 封包
            sendstring += protocol->Pack(reqjsonstr);
        }
        std::cout << sendstring << std::endl;

        // 3. 发请求送 -> 服务端
        sockfd->Send(sendstring);

        // 4. 接收应答 -> 接收服务端的
        sockfd->Recv(&inbuffer); // 读取的报文你也不能保证是完整的

        // 5. 解包并判断
        while(true)
        {
            std::string jsonrespstr;
            int n = protocol->UnPack(inbuffer, &jsonrespstr);
            if(n == 0)
            {
                LOG(LogLevel::DEBUG) << "解包完成";
                break;
            }

            // 6. 反序列化
            Response resp;
            resp.Deserialize(jsonrespstr);

            // 7. 打印结果
            std::cout << resp._result << "[" << resp._exitcode << "]" << std::endl;
        }
    }

    return 0;
}