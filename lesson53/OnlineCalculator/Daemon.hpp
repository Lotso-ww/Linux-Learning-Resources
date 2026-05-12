#ifndef __DAEMON__HPP
#define __DAEMON__HPP

#include <csignal>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// 将进程变成守护进程的函数
void Daemon(int nochagepath, int noredirect)
{
    // 0. 改变路径
    if(!nochagepath)
        chdir("/");

    // 1. 忽略相关信号
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    // 2. 我们需要保证自己不要成为组长
    if(fork() > 0)
    {
        // 父进程
        exit(0);
    }

    // 子进程
    // 3. 形成新的会话, 自己是话首进程
    setsid();

    // 4. 处理输入输出
    if (!noredirect) 
    {
      int fd = open("/dev/null", O_RDONLY);
      if (fd >= 0) 
      {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
      }
      close(fd);
    }
}
#endif