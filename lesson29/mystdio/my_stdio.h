#pragma once 

#define SIZE 1024

// 定义为 1,2,4方便使用 & 操作, 都只有一个比特位为 1
#define FLUSH_NONE 1
#define FLUSH_LINE 2
#define FLUSH_FULL 4

typedef struct 
{
    int fileno; // 文件描述符
    int flags;
    int fstrategy; // 刷新策略
    char outbuffer[SIZE];
    int cap; // 容量
    int size;
    //char inbuffer[1024];
}My_FILE;


My_FILE* Myfopen(const char* pathname, const char* mode); // r w a
int Myfwrite(const char* message, int size, int num, My_FILE* fp);
void Myfflush(My_FILE* fp);
void Myfclose(My_FILE* fp);
