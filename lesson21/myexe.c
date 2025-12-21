#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
    printf("我是父进程: pid: %d, ppid: %d\n", getpid(), getppid());
    pid_t id = fork();
    if(id == 0)
    {
        printf("我是子进程, pid: %d, ppid: %d\n", getpid(), getppid());
        sleep(1);
        char* const argv[] = {
            (char*)"top",
            (char*)"-d",
            (char*)"1",
            (char*)"-n",
            (char*)"3",
            NULL 
        };
        char* const env[] = {
            (char*)"haha = HAHA",
            (char*)"PATHd = xxx",
            (char*)"kid  = miney ",
            (char*)"moveud = normal",
            (char*)"most  = object"
        };

       // execvp(argv[0],argv);
       // execlp("ls", "ls", "-a", "-l",NULL);
       // execv("usr/bin/top", argv);
       // execl("usr/bin/ls", "ls", "-a", "-l",NULL);
       // execl("usr/bin/top", "top", "-d", "1", "-n", "3",NULL);
       execl("./myproc", "myproc", "-a", "-b", "-c", NULL);
       // execle("./myproc", "myproc", "-a", "-b", "-c", NULL, env);
       // extern char **environ;
       // putenv((char*)"haha=hehe");
       // putenv((char*)"class=118");
       // execle("./myproc", "myproc", "-a", "-b", "-c", NULL, environ); //我们没有传递环境变量！
        //execl("/usr/bin/bash", "bash", "shell.sh", NULL); //?????
        //execl("/usr/bin/python3", "python3", "test.py", NULL); //?????
        exit(1);
    }
    // father
    pid_t rid = waitpid(id, NULL, 0);
    if(rid > 0)
    {
        printf("wait child process success\n");
    }
    return 0;
}
//const int gnum = 5;
//
//
//void Work()
//{
//    int cnt = 5;
//    while(cnt)
//    {
//        printf("%d work..., cnt: %d\n", getpid(), cnt--);
//        sleep(1);
//    }
//}
//
//int main()
//{
//    std::vector<pid_t> subs;
//    for(int idx = 0; idx < gnum; idx++)
//    {
//        pid_t id = fork();
//        if(id < 0)
//            exit(1);
//        else if(id == 0)
//        {
//            //child
//            Work();
//            exit(0);
//        }
//        else
//        {
//            subs.push_back(id);
//        }
//    }
//
//    for(auto &sub : subs)
//    {
//        int status = 0;
//        pid_t rid = waitpid(sub, &status, 0);
//        if(rid > 0)
//        {
//            if(WIFEXITED(status))
//            {
//                printf("child quit normal, exit code: %d\n", WEXITSTATUS(status));
//            }
//            else
//            {
//                printf("%d child quit error!\n", sub);
//            }
//        }
//    }
//
//
//    return 0;
//}

//void PrintLog()
//{
//    printf("我要打印日志\n");
//}
//
//void SyncMySQL()
//{
//    printf("我要访问数据库!\n");
//}
//
//void Download()
//{
//    printf("我要下载核心数据\n");
//}
//
//typedef void(*task_t)();
//
//task_t tasks[3] = {
//    PrintLog,
//    SyncMySQL,
//    Download 
//};
//
//int main()
//{
//    printf("我是父进程, pid: %d, ppid: %d", getpid(), getppid());
//    pid_t id = fork();
//    if(id == 0)
//    {
//        // child
//        int cnt = 5;
//        while(cnt)
//        {
//            printf("我是子进程, pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt);
//            sleep(1);
//            cnt--;
//        }
//
//        exit(13);
//    }
//
//    while(1)
//    {
//        int status = 0;
//        pid_t rid = waitpid(id, &status, WNOHANG);
//        if(rid > 0)
//        {
//            if(WIFEXITED(status))
//            {
//                printf("子进程正常退出, 退出码: %d\n", WEXITSTATUS(status));
//            }
//            else
//            {
//                printf("进程异常退出,请注意!\n");
//            }
//            break;
//        }
//
//        else if(rid == 0)
//        {
//            sleep(1);
//            printf("子进程还没有退出,父进程轮询!\n");
//            for(int i = 0; i < 3; i++)
//            {
//                tasks[i]();
//            }
//        }
//        else{
//            printf("wait failed, who: %d, status: %d\n", rid, status);
//            break;
//        }
//    }
//        return 0;
//}
