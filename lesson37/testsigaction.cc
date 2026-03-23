#include <bits/types/sigset_t.h>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <signal.h>

void Printf(sigset_t& pending)
{
    for(int signum = 31; signum >= 1; signum--)
    {
        if(sigismember(&pending, signum))
        {
            printf("1");
        }
        else {
            printf("0");
        }
    }
    printf("\n");
}

void handler(int sig)
{
    std::cout << "get a signal " << sig << std::endl;
    
    // 在处理2号信号期间,2号信号被自动blcok了!
    while(true)
    {
        sigset_t pending;
        sigpending(&pending);

        Printf(pending);
        sleep(1);
    }
    // exit(0);
}

int main()
{
    struct sigaction act, old_act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&(act.sa_mask)); // ??? 进程收到了大量的重复信号？
    sigaddset(&(act.sa_mask), 1);
    sigaddset(&(act.sa_mask), 3);
    sigaddset(&(act.sa_mask), 4);
    sigaddset(&(act.sa_mask), 5);

    sigaction(2, &act, &old_act); // signal

    while(true)
    {
        pause(); //等待
    }
}