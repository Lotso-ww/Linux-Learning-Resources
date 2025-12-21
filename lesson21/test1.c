#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void PrintLog()
{
   printf("我要打印日志\n");
}

void SyncMySQL()
{
   printf("我要访问数据库!\n");
}

void Download()
{
   printf("我要下载核心数据\n");
}

typedef void(*task_t)();

task_t tasks[3] = {
   PrintLog,
   SyncMySQL,
   Download 
};

int main()
{
   printf("我是父进程, pid: %d, ppid: %d", getpid(), getppid());
   pid_t id = fork();
   if(id == 0)
   {
       // child
       int cnt = 5;
       while(cnt)
       {
           printf("我是子进程, pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt);
           sleep(1);
           cnt--;
       }

       exit(13);
   }

   while(1)
   {
       int status = 0;
       pid_t rid = waitpid(id, &status, WNOHANG);
       if(rid > 0)
       {
           if(WIFEXITED(status))
           {
               printf("子进程正常退出, 退出码: %d\n", WEXITSTATUS(status));
           }
           else
           {
               printf("进程异常退出,请注意!\n");
           }
           break;
       }

       else if(rid == 0)
       {
           sleep(1);
           printf("子进程还没有退出,父进程轮询!\n");
           for(int i = 0; i < 3; i++)
           {
               tasks[i]();
           }
       }
       else{
           printf("wait failed, who: %d, status: %d\n", rid, status);
           break;
       }
   }
       return 0;
}
