#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>

//// 正常退出，退出码 0
//int main() 
//{
//    printf("程序正常执行完毕\n");
//    return 0;
//}
//
//
//int main() 
//{
//    printf("执行exit退出\n");
//    exit(1);  // 退出码1，标识执行失败,也可以使用其他的退出码
//}
//
//int main() 
//{
//    printf("执行_exit退出");
//    // 故意不带 \n,体会刷没刷新缓冲区
//    _exit(1);  // 退出码1，标识执行失败,也可以使用其他的退出码
//}
//

//// exit会冲刷缓存，输出"hello"
//void TestExit() 
//{
//    printf("hello");
//    exit(0);
//}
//
//// _exit不冲刷缓存，无输出
//void Test_Exit() 
//{
//    printf("hello");
//    _exit(0);
//}
//
//
//int main()
//{
//    //TestExit();
//    Test_Exit();
//    return 0;
//}
//

int main() 
{
    int exit_code = 1;
    printf("退出码%d：%s\n", exit_code, strerror(exit_code));
    return 0;
}
