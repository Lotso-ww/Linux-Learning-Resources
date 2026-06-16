#include <memory>
#include <sys/epoll.h>
#include "Connection.hpp"
#include "Listener.hpp"
#include "Reactor.hpp"

static const int gport = 8080;

int main()
{
    // 1. listener -> 设置事件
    std::shared_ptr<Connection> connection = std::make_shared<Listener>(gport);
    connection->SetEvents(EPOLLIN | EPOLLET); // 设置读事件和ET的工作模式

    // 2. Reactor
    std::unique_ptr<Reactor> reactor = std::make_unique<Reactor>();
    
    // 3. reactor->AddConnection() -- 把listener加入进去
    reactor->AddConnection(connection);

    // 4. 开始派发事件
    reactor->DisPatcher();

    return 0;
}