#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    // 当前路径可以切换
   // chdir("/home/Lotso");
   // char pwd[64];
   // getcwd(pwd, sizeof(pwd));
   // printf("cwd: %s\n", pwd);
   
    FILE* fp = fopen("log.txt", "w");
   //FILE* fp = fopen("log.txt", "a");
   // FILE* fp = fopen("log.txt", "r");
   if(fp == NULL)
   {
       perror("fopen");
       return 0;
   }

   const char* message = "abcd\n";
   fputs(message, fp);
   int cnt = 0;
   while(cnt < 10)
   {
      // fwrite(message, 1, strlen(message), fp); // 不用加1, 建议不算\0;
      fputs(message, fp);
      intf(fp, "hello bit: % d\n", cnt);
      cnt++;
   }
   // 读取文件
 //  char inbuffer[1024];
 //  while(1)
 //  {
 //      // ftell 测试
 //     // long pos = ftell(fp);
 //     // printf("pos: %ld\n", pos);
 //     //  int ch = fgetc(fp);
 //     //  if(ch == EOF)
 //     //  {
 //     //     break;
 //     //  }
 //     //  printf("%c\n", ch);

 //      if(!fgets(inbuffer, sizeof(inbuffer),fp))
 //      {
 //          break;
 //      }
 //      printf("file: %s", inbuffer);
 //  }


}
