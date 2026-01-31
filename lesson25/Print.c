#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
    //printf("hello world"); // 语言缓冲区 -> stdout->outbuffer
    //sleep(40);
    
    // C 库函数
    printf("hello printf\n");
    fprintf(stdout, "hello fprint\n");
    const char* msg1 = "hello fputs\n";
    fputs(msg1, stdout);

    // 系统调用
    const char* msg2 = "hello write\n";
    write(1, msg2, strlen(msg2));

    fork(); // 最后
    return 0;
}

//int main()
//{
//    fprintf(stdout, "hello stdout\n");
//    fprintf(stderr, "hello error\n");
//
//    const char* msg1 = "hello 1\n";
//    const char* msg2 = "hello 2\n";
//    write(1, msg1, strlen(msg1));
//    write(2, msg2, strlen(msg2));
//
//    return 0;
//}

