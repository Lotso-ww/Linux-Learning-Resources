#include"process.h"
#include<string.h>

#define SIZE 103
#define LABEL '='


#define COLOR_GREEN  "\033[32m"   // 绿色
#define COLOR_GRAY   "\033[90m"   // 灰色
#define COLOR_CYAN   "\033[36m"   // 青色
#define COLOR_RESET  "\033[0m"    // 重置颜色

// 新增的颜色
#define COLOR_RED     "\033[31m"   // 红色
#define COLOR_YELLOW  "\033[33m"   // 黄色
#define COLOR_BLUE    "\033[34m"   // 蓝色
#define COLOR_MAGENTA "\033[35m"   // 品红
#define COLOR_WHITE   "\033[97m"   // 白色
#define COLOR_BLACK   "\033[30m"   // 黑色


//void Process_version(double total,double current,double speed,const char*userinfo)
//{
//    if(current > total)
//    {
//        return;
//    }
//
//    static const char *lable="|/-\\";
//    static int index = 0;
//    int size = strlen(lable);
//    // 1. 计算比率
//    double rate = current * 100.0  / total;
//    char out_bar[SIZE];
//    memset(out_bar, '\0', sizeof(out_bar));
//    // 2. 填充进度字符
//    int i = 0;
//    for(; i < (int)rate; i++)
//    {
//        out_bar[i] = LABEL;
//    }
//    // 3. 刷新进度条
//    printf("[%-100s][%5.1lf%%][%c] | %.1lf/%.1lf,speed: %.1lf%s\r", out_bar,rate,lable[index],current,total,speed,userinfo);
//    fflush(stdout);
//    index++;
//    index %= size;
//
//    // 4. 进度条完成,换行
//    if(current >= total)
//    {
//        printf("\r\n");
//    }
//}

void Process_version(double total,double current,double speed,const char*userinfo)
{
    if(current > total)
    {
        return;
    }

    static const char *lable="|/-\\";
    static int index = 0;
    int size = strlen(lable);
    // 1. 计算比率
    double rate = current * 100.0  / total;
    char out_bar[SIZE];
    memset(out_bar, '\0', sizeof(out_bar));
    // 2. 填充进度字符
    int i = 0;
    for(; i < (int)rate; i++)
    {
        out_bar[i] = LABEL;
    }
    // 3. 刷新进度条
    // printf("[%-100s][%5.1lf%%][%c] | %.1lf/%.1lf,speed: %.1lf%s\r", out_bar,rate,lable[index],current,total,speed,userinfo);
    printf(COLOR_RED"[%-100s]",out_bar);
    printf(COLOR_BLUE"[%5.1lf%%]",rate);
    printf(COLOR_CYAN"[%c]",lable[index]);
    printf(COLOR_YELLOW"| %.1lf/%.1lf,speed: %.1lf%s\r",current,total,speed,userinfo);
    printf(COLOR_RESET);
    fflush(stdout);
    index++;
    index %= size;

    // 4. 进度条完成,换行
    if(current >= total)
    {
        printf("\r\n");
    }
}
