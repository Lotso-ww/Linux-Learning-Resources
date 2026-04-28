#ifndef __ROUTE__HPP
#define __ROUTE__HPP

#include <vector>
#include "InetAddr.hpp"
#include "Logger.hpp"
#include "Mutex.hpp"
using namespace LogModule;

class Route 
{
private:
    bool IsOnline(const InetAddr &who)
    {
        for(auto& user : _users)
        {
            if(user == who)
            {
                return true;
            }
        }
        return false;
    }
    void Adduser(const InetAddr &who)
    {
        _users.push_back(who);
    }
public:
    Route()
    {}
    // 可以上引用试试
    void RouteMessage(std::string message, InetAddr who, int sockfd)
    {
        LOG(LogLevel::DEBUG) << "3.RouteMessage";
        // 临界区
        // 其实这样加锁的效率是比较低的, 我们可以采用拷贝的方式优化
        {
            LockGuard lockGuard(&_lock);
            // 1.检查用户是否在线上,在的话就不管了,不在的话就加入数组
            if(!IsOnline(who))
            {
                Adduser(who); // 加入数组管理起来
            }

            //2.转发逻辑 -- 我们在这个里面发
            for(auto& user : _users)
            {
                std::string sendMessage = who.StringAddress();
                sendMessage += "# ";
                sendMessage += message;
                sendto(sockfd, sendMessage.c_str(), sendMessage.size(), 0, user.Addr(), user.AddrLen());
            }
        }
    }
    ~Route()
    {}
private:
    std::vector<InetAddr> _users;
    Mutex _lock;
};
#endif 