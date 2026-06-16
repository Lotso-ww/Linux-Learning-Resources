#ifndef __COMMON__HPP
#define __COMMON__HPP

// 枚举错误退出：定义清晰的退出码，方便运维排查是哪一步出错
enum 
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_ERROR,
};

#endif
