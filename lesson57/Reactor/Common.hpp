#ifndef __COMMON__HPP
#define __COMMON__HPP

#include "Logger.hpp"
#include <fcntl.h>
using namespace LogModule;

// 枚举错误退出：定义清晰的退出码，方便运维排查是哪一步出错
enum 
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_ERROR,
};

void SetNonBlock(int fd)
{
    // 先获取已经有的标志位
    int flags = fcntl(fd, F_GETFL);
    if(flags < 0)
    {
        LOG(LogLevel::ERROR) << "fcntl error set: " << fd << " non block failed";
        return;
    }
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
#endif
