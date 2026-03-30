#include <iostream>
#include <signal.h>
#include <unistd.h>

int cnt = 0; 
void handler(int sig) 
{
    std::cout << "我正在处理一个进程, pid: " << getpid() << " sig number: " << sig << " cnt: " << cnt << std::endl;
    alarm(2);
    // exit(1); // 这里必须要退出,不然使用信号捕捉函数就是死循环
}
int main()
{
    alarm(1); // 1秒后发送SIGALRM
    // int n = alarm(0); // 取消闹钟,返回剩余时间
    signal(SIGALRM, handler);
    while(true)
    {
        std::cout << "cnt: " << cnt << std::endl;
        cnt++;
        sleep(1);
    }
    return 0;
}