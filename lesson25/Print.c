#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    // 打开文件
    FILE* fp = fopen("bite","w+");
    if(fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    // 向文件中写入 linux so easy!
    const char* msg = "linux so easy!\n";
    fwrite(msg, sizeof(char), strlen(msg), fp);

    // 移动文件光标回到开头,再进行读取
    fseek(fp, 0, SEEK_SET);

    // 读取文件
    char inbuffer[64] = {0};
    size_t s = fread(inbuffer, sizeof(char), sizeof(inbuffer) - 1, fp);
    if(s > 0)
    {
        printf("读取成功: %s", inbuffer);
    }
    else{
        printf("读取失败\n");
    }
    fclose(fp);
    return 0;
}
