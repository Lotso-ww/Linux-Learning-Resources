#include<iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

const int PAGE_SIZE = 4096; // 其实最后最小都是 4096,一定要是4096的倍数,否则会报错
// write_mmap filename
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }
    // 1.打开目标文件, mmap需要自己先打开文件
    int fd = ::open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(fd < 0)
    {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 2;
    }

    // 2. 我们需要手动调整一个文件的大小,方便我们进行合法的mmap
    if(::ftruncate(fd, PAGE_SIZE) < 0)
    {
        std::cerr << "Failed to ftruncate file: " << argv[1] << std::endl;
        return 3;
    }

    // 3. 进行mmap操作
    char *shmaddr = (char*)::mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shmaddr == MAP_FAILED)
    {
        std::cerr << "Failed to mmap file: " << argv[1] << std::endl;
        return 4;
    }

    // 4. 正在进行文件操作
    for (char c = 'a'; c <= 'z'; c++) 
    {
        shmaddr[c - 'a'] = c;
        sleep(1);
    }

    // 5. 关闭文件映射
    if(::munmap(shmaddr, PAGE_SIZE) == -1)
    {
        std::cerr << "Failed to munmap file: " << argv[1] << std::endl;
        return 5;
    }

    // 6. 关闭文件描述符
    ::close(fd);
    return 0;
}