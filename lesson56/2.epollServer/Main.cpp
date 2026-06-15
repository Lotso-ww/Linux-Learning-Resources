#include "epollServer.hpp"
#include <cstdint>
#include <memory>

const uint16_t gport = 8080;

int main()
{
    std::unique_ptr<epollServer> select_server= std::make_unique<epollServer>(gport);
    select_server->Dispatcher();
    return 0;
}