#include "selectServer.hpp"
#include <cstdint>
#include <memory>

const uint16_t gport = 8080;

int main()
{
    std::unique_ptr<selectServer> select_server= std::make_unique<selectServer>(gport);
    select_server->Start();
    return 0;
}