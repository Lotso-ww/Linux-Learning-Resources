#ifndef __HTTPSERVER__HPP
#define __HTTPSERVER__HPP

#include <cstdint>
#include <iostream>
#include <memory>
#include <numeric>
#include "TcpServer.hpp"
#include "Logger.hpp"
#include "HttpProtocol.hpp"

using namespace LogModule;


class HttpServer
{
public:
    HttpServer(uint16_t port)
        : _port(port)
        , _tsvr(std::make_unique<TcpServer>(port))
    {}

    std::string HandlerHttpRequest(std::string &streamstr)
    {
        // 先这样写测试看看, 后面调
        LOG(LogLevel::DEBUG) << "http request:\r\n";
        LOG(LogLevel::DEBUG) << "######################################\r\n";
        LOG(LogLevel::DEBUG) << streamstr;
        LOG(LogLevel::DEBUG) << "######################################\r\n";
    }

    void Run()
    {
        _tsvr->Run([this](std::string &streamstr){
            return this->HandlerHttpRequest(streamstr);
        });
    }

    ~HttpServer()
    {}
private:
    uint16_t _port;
    std::unique_ptr<TcpServer> _tsvr;
};


#endif