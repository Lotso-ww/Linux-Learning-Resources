// 非阻塞 I/O 读取测试程序
// 功能：演示如何以非阻塞方式从标准输入读取数据
// 特点：当数据未就绪时不会阻塞，而是执行其他任务后继续轮询

#include <cerrno>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

// ============================================================
// 函数功能：将指定文件描述符设置为非阻塞模式
// 参数：fd - 要设置为非阻塞的文件描述符
// 说明：通过 fcntl 获取并修改文件状态标志，添加 O_NONBLOCK
// ============================================================
void SetNoBlock(int fd)
{
    // 获取当前文件状态标志
    int flag = fcntl(fd, F_GETFL);
    if(flag < 0)
    {
        perror("fcntl");
        return;
    }
    // 设置非阻塞标志（保留原有标志，添加 O_NONBLOCK）
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

// ============================================================
// 函数功能：执行"其他任务"（模拟在数据未就绪时做其他工作）
// 说明：在实际应用中，这里可以处理其他 I/O 或业务逻辑
// ============================================================
void doOtherThing()
{
    std::cout << "0 not ready, do other thing!" << std::endl;
}

// ============================================================
// 主函数：非阻塞读取标准输入
// 流程：设置非阻塞 -> 循环读取 -> 处理各种返回值情况
// ============================================================
int main()
{
    // 定义输入缓冲区，大小为 128 字节
    char inbuffer[128];
    
    // 将标准输入（fd=0）设置为非阻塞模式
    SetNoBlock(0);
    
    // 持续轮询直到遇到 EOF 或发生错误
    while(true)
    {
        // 将缓冲区第一个字节设为 0（常量时间初始化）
        // 即使 read 失败，缓冲区也有确定状态
        inbuffer[0] = 0;
        
        // 尝试非阻塞读取，最多读取 sizeof(inbuffer) - 1 字节
        // 预留 1 字节空间用于 null 终止符
        ssize_t n = read(0, inbuffer, sizeof(inbuffer) - 1);
        
        // -----------------------
        // 情况1：成功读取到数据
        // -----------------------
        if(n > 0)
        {
            // 添加字符串终止符，确保可以安全打印
            inbuffer[n] = 0;
            std::cout << "inbuffer: " << inbuffer << std::endl;
        }
        // -----------------------
        // 情况2：读到 EOF（对方关闭了写端）
        // -----------------------
        else if(n == 0)
        {
            std::cout << "enter done, break" << std::endl;
            break;  // 正常退出循环
        }
        // -----------------------
        // 情况3：read 返回 -1（出错）
        // -----------------------
        else
        {
            // 1. 非阻塞模式下，只能以出错形式返回
            // 2. 底层没有数据，read返回，算不算出错？不算！！
            // 3. 如果返回值<0,你下来关系什么？？因为什么原因出错的？？错误码更详细的标识出错原因！
            // 4. errno == 11, EAGAIN || EWOULDBLOCK 不是真正的出错，只是表明fd没有就绪！
            
            // --- 假错误：数据未就绪 ---
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 执行其他任务，避免空轮询浪费 CPU
                doOtherThing();
                sleep(1);  // 暂停 1 秒再试（降低 CPU 占用）
                
                std::cout << "read 0, 0 fd data not ready!" << std::endl;
                continue;  // 继续下一次轮询
            }
            // --- 信号中断：被信号打断 ---
            else if(errno == EINTR)
            {
                // 系统调用被信号中断，可重试
                continue;
            }
            // --- 真错误：其他未知错误 ---
            else 
            {
                std::cout << "read error: n=" << n << ", errno=" << errno << std::endl;
                break;  // 发生未知错误，退出循环
            }
        }
        // 每次成功读取后暂停 1 秒
        sleep(1);
    }
    return 0;
}
