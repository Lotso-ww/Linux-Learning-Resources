#include"process.h"
#include<string.h>

// 进度条配置参数
#define SIZE 103     // 进度条缓冲区大小（适配100%+额外字符）
#define LABEL '='    // 进度填充字符

#define COLOR_GREEN  "\033[32m"   // 绿色
#define COLOR_GRAY   "\033[90m"   // 灰色
#define COLOR_CYAN   "\033[36m"   // 青色
#define COLOR_RESET  "\033[0m"    // 重置颜色
#define COLOR_RED     "\033[31m"   // 红色
#define COLOR_YELLOW  "\033[33m"   // 黄色
#define COLOR_BLUE    "\033[34m"   // 蓝色
#define COLOR_MAGENTA "\033[35m"   // 品红
#define COLOR_WHITE   "\033[97m"   // 白色
#define COLOR_BLACK   "\033[30m"   // 黑色


// 彩色动态进度条实现
void Process_version(double total,double current,double speed,const char*userinfo)
{
    // 边界处理：当前进度超过总进度时直接返回
    if(current > total)
    {
        return;
    }

    // 动态光标：循环切换，提示程序运行中
    static const char *lable="|/-\\";  
    static int index = 0;    // 静态变量，记录光标位置，避免每次重置
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
    // 3. 彩色打印进度条（分颜色区分不同部分，视觉更清晰）
    // printf("[%-100s][%5.1lf%%][%c] | %.1lf/%.1lf,speed: %.1lf%s\r", out_bar,rate,lable[index],current,total,speed,userinfo);
    printf(COLOR_RED"[%-100s]",out_bar);    // 进度主体（红色）
    printf(COLOR_BLUE"[%5.1lf%%]",rate);    // 百分比（蓝色）
    printf(COLOR_CYAN"[%c]",lable[index]);  // 动态光标（青色）
    printf(COLOR_YELLOW"| %.1lf/%.1lf,speed: %.1lf%s\r",current,total,speed,userinfo);  // 附加信息（黄色）
    printf(COLOR_RESET);  // 重置颜色，避免污染后续输出
    // 手动刷新缓冲区，确保内容实时显示
    fflush(stdout);
    // 更新光标索引（循环切换）
    index++;
    index %= size;

    // 4. 进度条完成,换行
    if(current >= total)
    {
        printf("\r\n");
    }
}

// 正常颜色进度条实现
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

