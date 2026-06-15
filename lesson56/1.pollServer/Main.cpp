#include "pollServer.hpp"
#include <cstdint>
#include <memory>

const uint16_t gport = 8080;

int main()
{
    std::unique_ptr<pollServer> select_server= std::make_unique<pollServer>(gport);
    select_server->Dispatcher();
    return 0;
}