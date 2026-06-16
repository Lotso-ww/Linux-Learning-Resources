#ifndef __IOHANDLER__HPP
#define __IOHANDLER__HPP

#include "Connection.hpp"

class IOHandler : public Connection
{
public:
    IOHandler(){}
    int Sockfd() override
    {
        return _sockfd;
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
    ~IOHandler(){}
private:
    int _sockfd;
};

#endif