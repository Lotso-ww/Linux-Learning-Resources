#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

// 定义进度条回调函数类型：适配不同场景的进度刷新逻辑
typedef void (*flush_t)(double total,double current,double speed,const char* userinfo);

// 彩色动态进度条核心接口
// total：总进度（如文件总大小）
// current：当前进度（如已下载大小）
// speed：当前速度（如MB/s）
// userinfo：附加信息（如单位）
void Process_version(double total,double current,double speed,const char*userinfo);
