#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "comm.h"

int main()
{
    int n = mkfifo(pathname, 0666);
    if(n < 0)
    {
        perror("mkfifo");
        return 1;
    }

    int rfd = open(pathname, O_RDONLY);
    if(rfd < 0)
    {
        perror("open");
        return 1;
    }

    char buf[1024];
    ssize_t rz = read(rfd, buf, sizeof(buf));
    if(rz < 0)
    {
        perror("read");
        return 1;
    }
    else if(rz == 0)
    {
        std::cout << "write end closed" << std::endl;
        return 0;
    }
    else{
        buf[rz] = '\0';
    }

    std::cout << "read: " << buf << std::endl;
    close(rfd);
    unlink(pathname);
    return 0;
}