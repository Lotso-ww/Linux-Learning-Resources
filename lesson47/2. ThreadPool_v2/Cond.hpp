#ifndef COND_HPP
#define COND_HPP
#include <iostream>
#include <pthread.h>
#include "Mutex.hpp"

class Cond 
{
public:
    Cond()
    {
        pthread_cond_init(&cond, nullptr);
    }
    void Wait(Mutex &mutex)
    {
        pthread_cond_wait(&cond, mutex.Origin());
    }
    void NotifyOne()
    {
        pthread_cond_signal(&cond);
    }
    void NotifyAll()
    {
        pthread_cond_broadcast(&cond);
    }
    ~Cond()
    {
        pthread_cond_destroy(&cond);
    }
private:
    pthread_cond_t cond;
};
#endif
