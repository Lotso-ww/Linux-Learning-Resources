#include <stdio.h>

int flag = 0; // 故意设置错误值

int Sum(int s, int e) 
{
    int result = 0;
    int i = s;
    for (; i <= e; i++) 
    {
        result += i;
    }
    return result * flag; // bug点: flag = 0 导致结果始终为 0 
}

int main() 
{
    int start = 1;
    int end = 100;
    printf("I will begin\n");
    int n = Sum(start, end);
    printf("running done, result is: [%d-%d]=%d\n", start, end, n);
    return 0;
}
