#pragma once

#include <iostream>
#include <semaphore.h>

class Sem
{
public:
    Sem(int num):_initNum(num)
    {
        sem_init(&sem, 0, _initNum);
    }
    void P()
    {
        sem_wait(&sem);
    }
    void V()
    {
        sem_post(&sem);
    }
    ~Sem()
    {
        sem_destroy(&sem);
    }
private:
    int _initNum;
    sem_t sem;
};