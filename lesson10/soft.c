#include<stdio.h>

void Version1()
{
    printf("version1: 免费\n");
}

void Version2()
{
    printf("versio2: 付费\n");
}

//#define FREE 1

int main()
{
#ifdef FREE 
    Version1();
#else 
    Version1();
    Version2();
#endif
    return 0;
}
