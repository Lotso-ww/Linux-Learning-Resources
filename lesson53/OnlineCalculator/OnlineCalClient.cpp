#include "Protocol.hpp"
#include <iostream>

void Usage(const std:: string procname)
{
    std::cout << "Usage" << procname << " local_port" << std::endl;
}
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    return 0;
}