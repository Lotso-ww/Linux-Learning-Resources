#include <iostream>
#include <pthread.h>


void* threadrun(void* arg)
{
    return nullptr;
}
int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, threadrun, (void*)"thread-1");
    // pthread_detach(tid);

    int n = pthread_join(tid, nullptr);
    std::cout << n << std::endl;
}