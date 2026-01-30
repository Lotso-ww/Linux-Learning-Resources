#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//int main()
//{
//    printf("stdin: %d\n", stdin->_fileno);
//    printf("stdout:%d\n", stdout->_fileno);
//    printf("stdout:%d\n", stderr->_fileno);
//    FILE *fp = fopen("log.txt", "w");
//    printf("log.txt: %d\n", fp->_fileno); // 3;
//    fclose(fp);
//
//    return 0;
//}

int main()
{
    printf("stdin: %d\n", stdin->_fileno);
    printf("stdout:%d\n", stdout->_fileno);
    printf("stdout:%d\n", stderr->_fileno);
    fprintf(stdout, "hello stdout\n");

    // close(1);
    int fda = open("loga.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    int fdb = open("logb.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    int fdc = open("logc.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);

    fprintf(stdout, "fda : %d\n", fda);
    fprintf(stdout, "fdb : %d\n", fdb);
    fprintf(stdout, "fdc : %d\n", fdc);
   // printf("fda : %d\n", fda);
   // printf("fdb : %d\n", fdb);
   // printf("fdc : %d\n", fdc);


    close(fda);
    close(fdb);
    close(fdc);
    return 0;
}

//int main()
//{
//    int fda = open("loga.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666); // w
//   // int fda = open("loga.txt", O_CREAT | O_WRONLY | O_APPEND, 0666); // a
//    dup2(fda, 1);
//
//    printf("hello printf\n"); // stdout -> 1
//    fprintf(stdout, "hello fprintf\n"); // stdout -> 1
//
//    close(fda);
//    return 0;
//}


//int main()
//{
//
//    int fda = open("loga.txt", O_RDONLY);
//    dup2(fda, 0);
//
//    int a = 0;
//    float f = 0.0f;
//    char c = 0;
//    scanf("%d %f %c", &a, &f, &c);
//    printf("%d, %f, %c\n", a, f, c);
//
//    close(fda);
//    return 0;
//}
