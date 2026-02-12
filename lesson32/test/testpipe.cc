#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

// 子进程 w
void WriteData(int wfd)
{
    int cnt = 1;
    char ch = 'a';
    // pid_t id = getpid();
    while(true)
    {
        // sleep(1);
        // std::string message = "hello father process, ";
        // message += "cnt: " + std::to_string(cnt++) + ", my pid is: " + std::to_string(id);
        // write(wfd, message.c_str(), message.size());

        sleep(1);
        write(wfd, &ch, 1);
        printf("cnt: %d\n", cnt++); // 64kb

        // sleep(5);
        // break;
    }
}

// 父进程：r
void ReadData(int rfd)
{
    char inbuffer[1024];
    while(true)
    {
        ssize_t n = read(rfd, inbuffer, sizeof(inbuffer) - 1);
        if(n > 0)
        {
            //sleep(5);
            inbuffer[n] = '\0';
            std::cout << getpid() << "# " << inbuffer << std::endl;
            sleep(3);
            break;
        }
        else if(n == 0)
        {
            printf("ReadData: pipe end: %ld\n", n);
            break;
        }
        else{
            printf("ReadData: pipe error: %ld\n", n);
            break;
        }
    }
}

int main()
{
    // 1. 创建管道成功
    int pipefd[2] = {0};
    int n = pipe(pipefd);
    (void)n;

    // 2. 创建子进程
    pid_t id = fork();
    if(id == 0)
    {
        // 3.形成单向通信的信道
        // 子进程: w
        close(pipefd[0]); // 关掉我不用的这个读
        WriteData(pipefd[1]);
        close(pipefd[1]); // 可以关掉
        exit(0);
    }
    else
    {
        // 3.形成单向通信的信道
        // 父进程: r
        close(pipefd[1]); // 关掉我不用的这个写
        ReadData(pipefd[0]);
        close(pipefd[0]); // 可以关掉

        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        (void) rid;

        printf("exit code: %d, exit signal: %d\n", (status >> 8) & 0xFF, status & 0x7F);
    }

    // 0->read 1->wirte
    // 0->嘴巴->读 1->笔->写
    // std::cout << "pipefd[0]: " << pipefd[0] << std::endl;
    // std::cout << "pipefd[1]: " << pipefd[1] << std::endl;
    return 0;
}
