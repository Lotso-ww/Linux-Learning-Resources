#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void waitChild(int signum)
{
    printf("pid: %d get a signal: %d\n", getpid(), signum);
    sleep(5);

    while(1)
    {
        pid_t rid = waitpid(-1, NULL, WNOHANG); // 非阻塞等待
        if(rid > 0)
        {
            printf("wait child: %d, success\n", rid);
        }
        else {
            break;
        }
    }
}
int main()
{
    signal(SIGCHLD, waitChild);
    for(int i = 0; i < 10; i++)
    {
        pid_t id = fork();
        // 子进程
        if(id == 0)
        {
            int cnt = 5;
            while(cnt)
            {
                printf("我是子进程: pid: %d, ppid: %d\n", getpid(), getppid());
                sleep(1);
                cnt--;
            }
            printf("子进程退出!\n");
            exit(0);
        }
    }

    // 父进程
    while(1)
    {
        printf("父进程完成任务!\n");
        sleep(1);
    }
    return 0;
}








// volatile int flag = 0;

// void handler(int signum)
// {
//     (void) signum;
//     flag = 1;
//     printf("更改flag 0->1\n");
// }


// int main()
// {
//     signal(2, handler);

//     while(!flag);

//     printf("进程正常退出\n");
//     return 0;
// }