#ifndef __LISTENER__HPP
#define __LISTENER__HPP

#include "Connection.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <memory>

class Listener : public Connection
{
public:
    Listener(uint16_t port)
        :_port(port)
        ,_listensockfd(std::make_unique<TcpSocket>())
    {
        _listensockfd->BuildSocketMethod(_port);
    }
    int Sockfd() override
    {
        return _listensockfd->Socketfd();
    }
    void Recver() override
    {

    }
    void Sender() override
    {

    }
    void Excepter() override
    {

    }
    ~Listener(){}
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensockfd;
};

#endif