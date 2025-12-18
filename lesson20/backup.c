#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
  // int i = 0;
  // for(; i < 255; i++)
  //     printf("%d->%s\n", i, strerror(i));
   printf("haha,I'm a process! pid: %d, ppid: %d\n", getpid(), getppid());
   return 88;
}

//int main()
//{
//    printf("我是父进程: pid: %d, ppid: %d\n", getpid(), getppid());
//
//    pid_t id = fork();
//    if(id < 0)
//    {
//        perror("fork");
//        exit(1);
//    }
//    if(id == 0)
//    {
//        // 子进程
//        int cnt = 5;
//        while(cnt)
//        {
//            printf("我是子进程: pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid());
//            sleep(1);
//            cnt--;
//        }
//        printf("子进程退出!\n");
//        exit(11);
//    }
//
//
//    // 父进程
//    // pid_t rid = wait(NULL);
//    int status = 0;
//    pid_t rid = waitpid(id, &status, 0);
//    if(rid > 0)
//    {
//        printf("等待子进程成功..., status: %d, exit code: %d\n", status, (status>>8)&0xFF);
//    }
//    return 0;
//
//    printf("hello world: %d", getpid());
//    sleep(2);
//    exit(14);
//    //_exit(16);
//}

//int print()
//{
//    printf("haha,I'm a process! pid: %d, ppid: %d\n", getpid(), getppid());
//    _exit(13);
//}
//int main()
//{
//    int n = print();
//    printf("n = %d\n", n);
//
//    return 10;
//    //return 10;
//   // while(1)
//   // {
//   //     printf("haha,I'm a process! pid: %d, ppid: %d\n", getpid(), getppid());
//   //     sleep(1);
//   //     //int a = 10;
//   //     //a/=0;
//   // }
//}
//
////int main()
////{
////    int i = 0;
////    for(; i < 255; i++)
////        printf("%d->%s\n", i, strerror(i));
////    printf("haha,I'm a process! pid: %d, ppid: %d\n", getpid(), getppid());
////    return 88;
////}
//
////#define NUM 10
////
////int data[NUM] = {0};
////
////void Backup()
////{
////    pid_t id = fork();
////    if(id == 0)
////    {
////        // child
////        int i = 0;
////        printf("Backup: ");
////        for(i = 0; i < NUM; i++)
////        {
////            printf("%d ", data[i]);
////        }
////        printf("\n");
////        sleep(10);
////        exit(0); // 进程结束
////    }
////}
////
////void ChangeData()
////{
////    int i = 0;
////    for(; i < NUM; i++)
////    {
////        data[i] = i + rand();
////    }
////    printf("origin data: ");
////    for(i = 0; i < NUM; i++)
////    {
////        printf("%d ", data[i]);
////    }
////    printf("\n");
////}
////
////int main()
////{
////    srand(time(NULL));
////    while(1)
////    {
////        //修改
////        ChangeData();
////        //备份
////        Backup();
////        sleep(5);
////    }
////}
