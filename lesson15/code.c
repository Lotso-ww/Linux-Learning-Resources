#include<stdio.h>
#include<unistd.h>


//int main()
//{
//    int a = 10;
//    scanf("%d",&a);
//    printf("hello %d\n",a);
//    return 0;
//}

struct obj
{
    int a;
    int b;
    char c;
    double d;
};

int main()
{
    struct obj x;
    printf("&x: %p, &(x.a): %p\n",&x,&(x.a));

    long long offset = (long long) & (((struct obj*)0)->d);
    long long start = (long)(long) & (x.d) - offset;

    printf("offset: %d\n",offset);

    printf("addr对比: %p,%p\n",&x,start);


    return 0;

}

