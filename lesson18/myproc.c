#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>


int g_unval;
int g_val = 100;

int main(int argc, char *argv[], char *env[])
{
    const char *str = "helloworld";
    // *str = 'H' // 错误
    printf("code addr: %p\n", main);
    printf("init global addr: %p\n", &g_val);
    printf("uninit global addr: %p\n", &g_unval);

    static int test = 10;
    char *heap_mem = (char*)malloc(10);
    char *heap_mem1 = (char*)malloc(10);
    char *heap_mem2 = (char*)malloc(10);
    char *heap_mem3 = (char*)malloc(10);
    printf("heap addr: %p\n", heap_mem); //heap_mem(0), &heap_mem(1)
    printf("heap addr: %p\n", heap_mem1); //heap_mem(0), &heap_mem(1)
    printf("heap addr: %p\n", heap_mem2); //heap_mem(0), &heap_mem(1)
    printf("heap addr: %p\n", heap_mem3); //heap_mem(0), &heap_mem(1)

    printf("test static addr: %p\n", &test); //heap_mem(0), &heap_mem(1)
    printf("stack addr: %p\n", &heap_mem); //heap_mem(0), &heap_mem(1)
    printf("stack addr: %p\n", &heap_mem1); //heap_mem(0), &heap_mem(1)
    printf("stack addr: %p\n", &heap_mem2); //heap_mem(0), &heap_mem(1)
    printf("stack addr: %p\n", &heap_mem3); //heap_mem(0), &heap_mem(1)

    printf("read only string addr: %p\n", str);
    for(int i = 0 ;i < argc; i++)
    {
        printf("argv[%d]: %p\n", i, argv[i]);
    }
    for(int i = 0; env[i]; i++)
    {
        printf("env[%d]: %p\n", i, env[i]);
    }

    return 0;
}

//int main()
//{
//    printf("PATH: %s\n", getenv("PATH"));
//    printf("HOME: %s\n", getenv("HOME"));
//    printf("PWD: %s\n", getenv("PWD"));
//    printf("HOSTNAME: %s\n", getenv("HOSTNAME"));
//    printf("OK: %s\n", getenv("OK"));
//    
//
//    return 0;
//}



//int main()
//{
//    // environ
//    extern char **environ;
//    int i = 0;
//    for(; environ[i]; i++)
//    {
//        printf("environ[%d]: %s\n", i, environ[i]);
//    }
//
//    return 0;
//}


//int main(int argc,char *argv[],char *env[])
//{
//    (void)argc;
//    int i = 0;
//    for(; argv[i]; i++)
//    {
//        printf("argv[%d]: %s\n", i, argv[i]);
//    }
//    printf("----------------------------------\n");
//    for(int i = 0; env[i]; i++)
//    {
//        printf("env[%d]: %s\n", i, env[i]);
//    }
//
//    return 0;
//}

//int main()
//{
//    char *who = getenv("USER");
//    if(strcmp(who,"Lotso") == 0)
//    {
//        printf("我已经是命令,变成了进程执行\n");
//    }
//    else 
//    {
//        printf("权限错误, 不认识你: %s\n",who);
//    }
//
//    return 0;
//}




//int main(int argc,char* argv[])
//{
//   // (void)argc;
//   // if(strcmp(argv[1],"-a") == 0)
//   // {
//   //     printf("这是我的程序的功能1\n");
//   // }
//   // else if(strcmp(argv[1],"-b") == 0)
//   // {
//   //     printf("这是我的程序的功能2\n");
//   // }
//   // else 
//   // {
//   //     printf("这是我的程序的默认功能\n");
//   // }
//
//   // int i = 0;
//   // for( ; i < argc; i++)
//   // {
//   //     printf("argv[%d]: %s\n",i,argv[i]);
//   // }
//
//    return 0;
//}
