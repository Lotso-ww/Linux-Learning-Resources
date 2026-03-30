#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "comm.h"
#include <cstring>

int main()
{
    int wfd = open(pathname, O_WRONLY);
    if(wfd < 0)
    {
        perror("open");
        return 1;
    }

    const char* msg = "I am process A";
    ssize_t wz = write(wfd, msg, strlen(msg));
    if(wz < 0)
    {
        perror("write");
        return 1;
    }

    return 0;
}