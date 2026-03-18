#include <iostream>
#include <string>
#include <signal.h>

void Usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " signumber pid\n\n";
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }

    int signumber = std::stoi(argv[1]);
    int pid = std::stoi(argv[2]);

    std::cout << "send " << signumber << " to " << pid << std::endl;
    int n = kill(pid, signumber);
    (void)n;
    
    return 0;
}