#include <stdio.h>
#include <stdlib.h>

int main() 
{
    char *var1 = getenv("MYENV1");
    char *var2 = getenv("MYENV2");
    
    printf("MYENV1：%s\n", var1 ? var1 : "未找到");
    printf("MYENV2：%s\n", var2 ? var2 : "未找到");
    return 0;
}
