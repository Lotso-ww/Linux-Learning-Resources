#include <memory>
#include <sys/epoll.h>
#include "Calculator.hpp"
#include "Connection.hpp"
#include "Listener.hpp"
#include "Protocol.hpp"
#include "Reactor.hpp" 

static const int gport = 8080;

int main()
{
    // 1. 业务层: 计算器
    std::unique_ptr<Calculator> cal = std::make_unique<Calculator>();

    // 2. 协议层: Protocol
    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>(
        [&cal](const Request& req){
            return cal->Exec(req);
    });

    // 3. listener -> 设置事件, 还有一件事就是和前面的关联起来, 也是设置回调, 再往下可以透到IOHandler(里面要用上)
    std::shared_ptr<Connection> connection = std::make_shared<Listener>(gport, 
        [&protocol](std::string &inbuffer, int *code){
            return protocol->HandlerRequest(inbuffer, code);
    });
    connection->SetEvents(EPOLLIN | EPOLLET); // 设置读事件和ET的工作模式

    // 4. Reactor
    std::unique_ptr<Reactor> reactor = std::make_unique<Reactor>();
    
    // 5. reactor->AddConnection() -- 把listener加入进去
    reactor->AddConnection(connection);

    // 6. 开始派发事件
    reactor->DisPatcher();

    return 0;
}