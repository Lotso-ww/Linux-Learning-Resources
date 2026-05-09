#include "Protocol.hpp"

int main()
{
    // 测试序列化
    class Request request(10, 10, '+');
    std::string s;
    request.Serialize(&s);
    std::cout << s << std::endl;

    // 测试反序列化
    // std::string jsonstring = "{\"datax\":-30,\"datay\":100,\"oper\":43}";
    // Request req(0, 0, '-');
    // req.Print();
    // std::cout << "-----------------" << std::endl;
    // req.Deserialize(jsonstring);
    // req.Print();
    
    return 0;
}