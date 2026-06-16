#ifndef __CONNECTION__HPP
#define __CONNECTION__HPP

#include <cstdint>
#include <string>
#include "InetAddr.hpp"

// 基类! -- 先描述
class Connection 
{
public:
    Connection():_events(0)
    {}
    uint32_t Events() { return _events; }
    void SetEvents(uint32_t events) { _events = events; }
    virtual int Sockfd() = 0;
    virtual void Recver() = 0;
    virtual void Sender() = 0;
    virtual void Excepter() = 0;
    ~Connection(){}
protected: // 保护成员
    std::string inbuffer;  // 输入缓冲区
    std::string outbuffer; // 输出缓冲区

    InetAddr clientaddr; // client addr
    uint32_t _events;
};

#endif