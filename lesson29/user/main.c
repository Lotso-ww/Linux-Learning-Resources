#include "my_stdio.h"
#include "my_string.h"
#include <stdio.h>

int main()
{
    // Myfopen这里就不再测试了
    
    const char* msg = "hello world";
    printf("msg len : %d\n", my_strlen(msg));
}
