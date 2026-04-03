#include<iostream>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

// 极简malloc实现
void* my_malloc(size_t size)
{
    if(size > 0)
    {
        void* addr = (void*)::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(addr == MAP_FAILED)
        {
            std::cerr << "Failed to mmap " << size << std::endl;
            return nullptr;
        }
        return addr;
    }
    return nullptr;
}

void my_free(void* start, size_t size)
{
    if(start != nullptr && size > 0)
    {
        int ret = ::munmap(start, size);
        if(ret == -1)
        {
            std::cerr << "Failed to munmap " << size << std::endl;
        }
    }
}
int main()
{
    char* p = (char*)my_malloc(1024);
    if(p == nullptr)
    {
        std::cerr << "Failed to malloc 1024 bytes" << std::endl;
        return 1;
    }
    // 使用分配的内存,简单打印指针值
    printf("Allocated memory at address: %p\n", p);
    // 在这里使用ptr指向的内存
    memset(p, 'A', 1024);

    for(int i = 0; i < 1024; i++)
    {
        printf("%c ", p[i]);
        fflush(stdout);
        sleep(1);
    }

    // 释放内存
    my_free(p, 1024);
    return 0;
}