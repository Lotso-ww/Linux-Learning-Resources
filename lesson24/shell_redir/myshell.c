#include "myshell.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

// 提示符相关
static char username[32];
static char hostname[64];
static char cwd[256];
static char commandLine[256];
// 与命令行相关
static char* argv[64];
static int argc = 0;
static const char* sep = " ";
// 与退出码有关
static int lastCode = 0;
// 与环境变量相关,按道理来说是由bash来维护的,从系统配置文件读,但是我们这里直接从系统bash拷贝就行了
char** _environ;
static int envc = 0;
// 重定向相关
// ls -a -l > test.txt 
#define NoneRedir 0
#define InputRedir 1
#define OutputRedir 2
#define AppRedir 3
static int redir_type = NoneRedir;
static char* redir_filename = NULL;

#define CLEAR_LEFT_SPACE(pos) do{while(isspace(*pos)) pos++; }while(0)

static void InitEnv()
{
    extern char** environ; // 系统环境变量数组（以NULL结尾）
    for(envc = 0; environ[envc]; envc++)
    {
        _environ[envc] = environ[envc];
    }
}

static void PrintAllEnv()
{
    int i = 0;
    for( ; _environ[i]; i++ )
    {
        printf("%s\n", _environ[i]);
    }
}

static void AddEnv(const char* val) // argv[1];
{
    _environ[envc] = (char*)malloc(strlen(val) + 1);
    strcpy(_environ[envc], val);
    _environ[++envc] = NULL; 
}

static void GetUserName()
{
    char* _username = getenv("USER");
    strcpy(username, (_username ? _username : "None"));
}

static void GetHostName()
{
    char* _hostname = getenv("HOSTNAME");
    strcpy(hostname, (_hostname ? _hostname : "None"));
}

static void GetCmd()
{
   // char* _cwd = getenv("PWD");
   //  strcpy(cwd, (_cwd ? _cwd : "None"));
   char _cwd[256];
   getcwd(_cwd, sizeof(_cwd));
   if(strcmp(_cwd, "/") == 0)
   {
       strcpy(cwd, _cwd);
   }
   else{
       int end = strlen(_cwd) - 1;
       while(end >= 0)
       {
           if(_cwd[end] == '/')
           {
               strcpy(cwd, &_cwd[end + 1]);
               break;
           }
           end--;
       }
   }
}

static void PrintPromt()
{
    GetUserName();
    GetHostName();
    GetCmd();
    printf("[%s@%s %s]# ",username, hostname, cwd);
    fflush(stdout);
}

static void GetCommandLine()
{
    if(fgets(commandLine, sizeof(commandLine), stdin) != NULL)
    {
        commandLine[strlen(commandLine) - 1] = 0;
    }
}
// 1. yes
// 0. no, 普通命令，让后续的执行
int CheckBuiltinAndExcute()
{
    int ret = 0;
    if(strcmp(argv[0], "cd") == 0)
    {
        // 内键命令
        ret = 1;
        if(argc == 2) // 后面至少需要跟个东西
        {
            chdir(argv[1]);
        }
    }
    else if(strcmp(argv[0], "echo") == 0)
    {
        ret = 1;
        if(argc == 2)
        {
            if(argv[1][0] == '$')
            {
                if(strcmp(argv[1], "$?") == 0)
                {
                    printf("%d\n", lastCode);
                    lastCode = 0;
                }
                else{
                    // env 
                }
            }
            else
            {
                printf("%s\n", argv[1]);
            }
        }
    }
    else if(strcmp(argv[0], "env") == 0)
    {
        ret = 1;
        PrintAllEnv();
    }
    else if(strcmp(argv[0], "export") == 0)
    {
        ret = 1;
        if(argc == 2)
        {
            AddEnv(argv[1]);
        }
    }

    return ret;
}

void Excute()
{
    pid_t id = fork();
    if(id < 0)
    {
        perror("fork");
        return;
    }
    else if(id == 0)
    {
        // 子进程
        // 程序替换
        // 要不要重定向,怎么重定向
        // filename 
        if(redir_type == InputRedir)
        {
            int fd = open(redir_filename, O_RDONLY);
            (void)fd;
            dup2(fd, 0);
        }
        else if(redir_type == OutputRedir)
        {
            int fd = open(redir_filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            (void)fd;
            dup2(fd, 1);
        }
        else if(redir_type == AppRedir)
        {
            int fd = open(redir_filename, O_CREAT | O_WRONLY | O_APPEND, 0666);
            (void)fd;
            dup2(fd, 1);
        }
        else{
            // do nothing
        }
            execvp(argv[0], argv);
            exit(1);
     }
    else{
        // 父进程
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        (void)rid;
        lastCode = WEXITSTATUS(status);
    }
}
static void ParseCommandLine()
{
     // 清空
     argc = 0;
     memset(argv, 0, sizeof(argv));
     // 判空
     if(strlen(commandLine) == 0) return;
     // 分割
     argv[argc] = strtok(commandLine, sep);
     while((argv[++argc] = strtok(NULL, sep)));
}

void Redir()
{
    // 核心目标
    // "ls -a -l >> > < filename"
    // redir_filename = filename 
    // redir_type = InputRedir 
    char* start = commandLine;
    char* end = commandLine + strlen(commandLine);
    while(start < end)
    {
        // > >> <
        if(*start == '>')
        {
            if(*(start + 1) == '>')
            {
                // 追加重定向
                redir_type = AppRedir;
                *start = 0;
                start += 2;
                CLEAR_LEFT_SPACE(start);
                redir_filename = start;
                break;
            }
            else{
                // 输出重定向
                redir_type = OutputRedir;
                *start = '\0';
                start++;
                CLEAR_LEFT_SPACE(start);
                redir_filename = start;
                break;
            }
        }

        else if(*start == '<') 
        {
            // 输入重定向
            redir_type = InputRedir;
            *start = '\0';
            start++;
            CLEAR_LEFT_SPACE(start);
            redir_filename = start;
            break;
        }
        else{
            start++;
        }
    }
}
void bash()
{
    // 环境变量相关,方便实现通过声明(_environ)就能直接用环境变量
    static char* env[64];
    _environ = env;
    // 除此以外我们还可以通过一个数组存储本地变量
    // 以及可以通过一个来存储别名…
    // 初始化读取环境变量
    InitEnv();
    while(1)
    {
        // 每次开始前重置一下重定向文件和状态
        redir_type = NoneRedir;
        redir_filename = NULL;
        // 第一步: 输出提示命令行
        PrintPromt();

        // 第二步: 等待用户输入, 获取用户输入
        GetCommandLine();

        // "ls -a -l > filename" -> "ls -a -l" "filename" redir_type
        // 2.1
        Redir();

        // 第三步: 解析字符串,"ls -a -l" -> "ls" "-a" "-l"
        ParseCommandLine();

        if(argc == 0)
            continue;

        // 第四步: 有些命令, cd echo env等等不应该让子进程执行
        // 而是让父进程自己执行,这些是内建命令. bash内部的函数
        if(CheckBuiltinAndExcute())
            continue;

        // 第五步: 执行命令
        Excute();
    }
}
