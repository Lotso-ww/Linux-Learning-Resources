#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int g_val = 0;  // 全局变量

int main() 
{
    pid_t id = fork();  // 创建子进程
    if (id < 0) 
    {
        perror("fork failed");
        return 1;
    } 
    else if (id == 0) 
    {  // 子进程
        g_val = 100;  // 子进程修改全局变量
        printf("子进程[PID:%d]：g_val=%d，地址=%p\n", getpid(), g_val, &g_val);
    } 
    else 
    {  // 父进程
        sleep(3);  // 等待子进程修改完成
        printf("父进程[PID:%d]：g_val=%d，地址=%p\n", getpid(), g_val, &g_val);
    }
    sleep(1);
    return 0;
}
