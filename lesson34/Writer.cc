// header only
#include "Shm.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

Shm shm;
class Init
{
public:
    Init()
    {
        shm.Get();
        addr = (char*)shm.Attch();
        std::cout << "addr: " << ToHex((long long)addr) << std::endl;
    }
    ~Init()
    {
        shm.Detach();
    }
    char *Addr()
    {
        return addr;
    }
public:
    char* addr;
};

Init init;
// Write.
int main()
{
    std::cout << "test begin..." << std::endl;

    buffer_t *shmbuf = (buffer_t*)init.Addr();
    shmbuf->count = 0;
    memset(shmbuf->buf, 0, 4096);

    char ch = 'A';
    for(int i = 0; i < 26 * 2; i += 2, ch++)
    {
        shmbuf->buf[i] = ch;
        usleep(234219);
        shmbuf->buf[i + 1] = ch;
        usleep(734217);
        shmbuf->count++;
        usleep(734217);

        sleep(1);
    }
    return 0;
}