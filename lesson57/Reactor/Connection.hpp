#ifndef __CONNECTION__HPP
#define __CONNECTION__HPP

#include <cstdint>
#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace LogModule;
class Reactor;

// 基类! -- 先描述
class Connection 
{
public:
    Connection():_events(0)
    {}
    uint32_t Events() { return _events; }
    void SetEvents(uint32_t events) { _events = events; }
    void SetClientAddress(InetAddr& addr) { _clientaddr = addr; }
    virtual int Sockfd() = 0;
    virtual void Recver() = 0;
    virtual void Sender() = 0;
    virtual void Excepter() = 0;
    ~Connection(){}
protected: // 保护成员
    InetAddr _clientaddr; // client addr
    uint32_t _events;    // 关心的事件
public:
    Reactor* _R; // 回指指针 -> 反向找到Reactor
};

#endif