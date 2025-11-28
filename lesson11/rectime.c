#include<stdio.h>
#include<unistd.h>

int main()
{
    int cnt = 10;
    for(;cnt >= 0;cnt--)
    {
        printf("倒计时: %-2d\r",cnt);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    return 0;
}
