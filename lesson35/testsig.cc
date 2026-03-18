#include <iostream>
#include <string>
#include <unistd.h>
#include <signal.h>


// sig: 表示的是收到的信号编号
void handlersig(int sig)
{
    std::cout << "哈哈, 我正在处理一个信号, pid: " << getpid() << " sig number: " << sig << std::endl;
}

int main()
{
    for(int signo = 0; signo <= 31; signo++)
        signal(signo, handlersig);
    // signal(2, handlersig);
    // signal(2, SIG_DFL);
    // signal(2, SIG_IGN);
    // signal(3, handlersig);
    // signal(4, handlersig);
    // signal(11, handlersig);

    int a;
    while(true)
    {
        // scanf("%d", &a);
        std::cout << "我是一个进程, pid: " << getpid() << std::endl;
        sleep(1);
        // int *p = nullptr;
        // *p = 100; // 野指针报错 

        // int a = 10;
        // a /= 0; // 除0错误

        // raise(SIGINT); // 指定信号给当前进程
        // abort(); // 是6号信号给当前进程
    }
}