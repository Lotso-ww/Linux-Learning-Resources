#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() 
{
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork failed");
        return 1;
    } 
    else if (pid == 0) 
    { // 子进程
        sleep(5);
        exit(1);
    } 
    else 
    { // 父进程非阻塞等待
        int status;
        pid_t ret;
        do {
            ret = waitpid(pid, &status, WNOHANG); // 非阻塞
            if (ret == 0) 
            {
                printf("子进程仍在运行，父进程可处理其他任务...\n");
                sleep(1); // 模拟父进程其他工作
            }
        } while (ret == 0); // 直到回收成功或失败
        
        if (WIFEXITED(status)) 
        {
            printf("子进程退出，退出码：%d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
