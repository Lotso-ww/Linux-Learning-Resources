#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

typedef void (*flush_t)(double total,double current,double speed,const char* userinfo);
void Process_version(double total,double current,double speed,const char*userinfo);
