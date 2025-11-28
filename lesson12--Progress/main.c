#include"process.h"
#include<unistd.h>
#include<stdlib.h>
#include<time.h>


// 全局配置（可根据实际场景修改）
double gtotal = 1024.0;  // 默认总进度（如1024MB）
double gspeed = 1.0;     // 默认速度

// 模拟下载任务：调用进度条回调函数刷新进度
void Download(double total,flush_t cb)
{
    double current = 0.0;
    // 模拟不同网络速度（模拟实际场景中速度波动）
    double level[] = {0.01,0.05,10.0,20.0,24.0,38.0,50.0,68.9};
    int num = sizeof(level)/sizeof(level[0]);
    while(1)
    {
        // 模拟下载耗时（1秒刷新一次）
        usleep(1000000);

        // 随机选择当前速度（模拟网络波动）
        double speed = level[rand()%num];
        current += speed;

        // 边界处理：进度达到100%时终止
        if(current >= total)
        {
            current = total;  // 确保进度不超过100%
            cb(total,current,speed,"MB/s");
            break;
        }
        else{
            // 刷新进度条
            cb(total,current,speed,"MB/s");
        }
    }
    // 这个跟proces.c中的第4步效果类似,写那个都行
    //printf("\n");
}
int main()
{
    // 初始化随机数种子，模拟速度波动
    srand(time(NULL));
	// 测试4个不同大小的下载任务，验证进度条适配性
    printf("download: \n");
    Download(gtotal,Process_version);

    printf("download: \n");
    Download(102.0,Process_version);

    printf("download: \n");
    Download(110.9,Process_version);

    printf("download: \n");
    Download(900.0,Process_version);

    return 0;
}
