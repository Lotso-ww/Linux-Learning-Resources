#include"process.h"
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

// 有一个下载任务
double gtotal = 1024.0;
double gspeed = 1.0;

void Download(double total,flush_t cb)
{
    double current = 0.0;
    double level[] = {0.01,0.05,10.0,20.0,24.0,38.0,50.0,68.9};
    int num = sizeof(level)/sizeof(level[0]);
    while(1)
    {
        usleep(1000000);
        double speed = level[rand()%num];
        current += speed;
        if(current >= total)
        {
            current = total;
            cb(total,current,speed,"MB/s");
            break;
        }
        else{
            cb(total,current,speed,"MB/s");
        }
    }

}
int main()
{
    srand(time(NULL));
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
