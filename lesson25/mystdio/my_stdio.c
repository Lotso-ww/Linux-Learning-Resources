#include "my_stdio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static mode_t fmode = 0666; // 文件权限

My_FILE* Myfopen(const char* pathname, const char* mode) // r w a
{
    if(pathname == NULL || mode == NULL)
        return NULL;
    umask(0);
    int fd = 0;
    int flags = 0;
    if(strcmp(mode, "w") == 0)
    {
        flags = O_CREAT | O_WRONLY | O_TRUNC;
        fd = open(pathname, flags, fmode);
        (void) fd;
    }
    if(strcmp(mode, "r") == 0)
    {
        flags = O_RDONLY;
        fd = open(pathname, flags);
        (void) fd;
    }
    if(strcmp(mode, "a") == 0)
    {
        flags = O_CREAT | O_WRONLY | O_APPEND;
        fd = open(pathname, flags, fmode);
        (void) fd;
    }
    else{}

    if(fd < 0) return NULL;
    // 创建 My_FILE对象
    My_FILE* fp = (My_FILE*)malloc(sizeof(My_FILE));
    if(!fp) return NULL;
    fp->fileno = fd;
    fp->flags = flags;
    fp->fstrategy = FLUSH_LINE;
    fp->outbuffer[0] = '\0';
   // memset(fp->outbuffer, 0, sizeof(fp->outbuffer));
   fp->cap = SIZE;
   fp->size = 0;
   return fp;
}


void Myfflush(My_FILE* fp)
{
    if(!fp) return;
    if(fp->size > 0)
    {
        // 写到内核文件的文件缓冲区中
        write(fp->fileno, fp->outbuffer, fp->size);
        // 刷新到外设
        fsync(fp->fileno);
        fp->size = 0;
    }
}

int Myfwrite(const char* message, int size, int num, My_FILE* fp)
{
    if(message == NULL || fp == NULL) return 0;

    // C语言向文件写入实际上是向缓冲区写入
    int sizeNum = size * num;
    if(fp->size + sizeNum < fp->cap - 1) // 预留\0的位置
    {
        memcpy(fp->outbuffer + fp->size, message, sizeNum);
        fp->size += sizeNum;
        fp->outbuffer[fp->size] = 0;
    }

    // 刷新缓冲区条件: 不是每次都刷新的,这样也可以加快响应速度
    if(fp->size > 0 && fp->outbuffer[fp->size - 1] == '\n' && (fp->fstrategy & FLUSH_LINE))
    {
        Myfflush(fp);
    }

    return sizeNum;
}


void Myfclose(My_FILE* fp)
{
    if(fp->size > 0)
    {
        Myfflush(fp);
    }
    close(fp->fileno);
}
