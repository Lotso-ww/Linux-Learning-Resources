#ifndef __HTTPSERVER__HPP
#define __HTTPSERVER__HPP

#include <cstdint>
#include <iostream>
#include <memory>
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
        // 1. 检查报文完整性 -- 我们今天默认报文是完整的, 这里就不处理了
        // 2. 对收到的请求进行反序列化
        HttpRequest httpreq;
        httpreq.Deserialize(streamstr);

        // 拿到httpreq里面的结构化数据 -- 后面就当然可以操作
        std::string target = httpreq["path"]; // /a/b/c.html
        std::cout << "http req: " << target << std::endl;

        // 3. httpreq -> httpresp
        // TODO 我们后面再来实现这里的
        HttpResponse httpresp;

        // 4. 应答进行序列化
        std::string httprespstr;
        httpresp.Serialize(&httprespstr); // 带出来

        return httprespstr;

        //**************************前期测试使用******************************//
        // HttpRequest req;
        // // Unpack(streamstr);
        // req.Deserialize(streamstr);
        // LOG(LogLevel::DEBUG) << "http request:\r\n";
        // LOG(LogLevel::DEBUG) << "######################################\r\n";
        // LOG(LogLevel::DEBUG) << "\r\n";
        // LOG(LogLevel::DEBUG) << streamstr;
        // LOG(LogLevel::DEBUG) << "######################################\r\n";

        // std::string resp_status = "HTTP/1.0 200 OK\r\n";
        // std::string resp_content = "hello world, hello http!";
        // std::string cl = "Content-Length: " + std::to_string(resp_content.size()) + "\r\n";
        // std::string blankLine = "\r\n";
        // return resp_status + cl + blankLine + resp_content;
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