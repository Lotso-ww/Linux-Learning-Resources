#include<stdio.h>
#include<unistd.h>

int main()
{
    while(1)
    {
        sleep(1);
        printf("我是一个进程, pid: %d,ppid: %d\n",getpid(),getppid());
    }

    return 0;
}
