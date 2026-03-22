#include <bits/types/sigset_t.h>
#include <csignal>
#include <iostream>
#include <cstdio>
#include <ostream>
#include <signal.h>
#include <unistd.h>

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
    std::cout << "处理了" << sig << "号信号" << std::endl;

    // 测试: pending信号被递达的时候，是跑handler之前还是之后1 -> 0
    std::cout << "***************" <<std::endl;
    sigset_t pending;
    sigemptyset(&pending); // 不会对后续有影响,只是在栈上
    int m = sigpending(&pending);
    (void)m;
    Printf(pending);
    std::cout << "***************" <<std::endl;
}
int main()
{
    std::cout << "pid: " << getpid() << std::endl;
    // 0. 自定义捕获2号信号
    signal(SIGINT, handler);
    // 1. 定义&&初始化信号集（定义变量而已，没多高级，在用户栈上）
    sigset_t block, old_block;
    sigemptyset(&block); // 全设为0先
    sigemptyset(&old_block);

    for(int signum = 1; signum <= 31; signum++)
    {
        sigaddset(&block, signum);
    }
    // 2. 向block信号集添加2号信号
    // sigaddset(&block, SIGINT);

    // 3. 屏蔽2号信号
    int n = sigprocmask(SIG_SETMASK, &block, &old_block);
    (void)n;

    // 4. 不断获取pending信号集 && 打印出来观察
    sigset_t pending;
    int cnt = 0;
    while(true)
    {
        sigemptyset(&pending);

        // 获取pending信号集
        int m = sigpending(&pending);
        (void)m;

        // 打印pending信号集进行观察
        Printf(pending);

        sleep(1);
        // 尝试恢复
        cnt++;
        if(cnt == 20)
        {
            std::cout << "恢复2号信号, 解除屏蔽" << std::endl;
            sigprocmask(SIG_SETMASK, &old_block,nullptr); // 恢复
        }
    }
}