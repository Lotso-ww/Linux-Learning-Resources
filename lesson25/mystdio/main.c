#include "my_stdio.h"
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    My_FILE* fp = Myfopen("log.txt", "a");
    if(!fp) return 1;

    const char* msg = "hello Lotso \n";
    int cnt = 10;
    while(cnt--)
    {
        Myfwrite(msg, 1, strlen(msg), fp);
       //  Myfflush(fp); // 如果没有换行
        sleep(2);
    }

    Myfclose(fp);
    return 0;
}
