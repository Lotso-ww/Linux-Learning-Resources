#ifndef __EXCUTECOMMAND__HPP
#define __EXCUTECOMMAND__HPP

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <vector>
#include "Logger.hpp"
using namespace std;
using namespace LogModule;

class ExcuteCommand 
{
private:
    bool IsSafe(const std::string &cmdstr)
    {
        for(auto& str : _white_list)
        {
            if(cmdstr == str) return true;
        }

        return false;
    }
public:
    ExcuteCommand()
    {
        _white_list.push_back("pwd");
        _white_list.push_back("who");
        _white_list.push_back("whoami");
        _white_list.push_back("ls -a -l");
        _white_list.push_back("env");
    }
    std::string Excute(const std::string cmdstr)
    {
        if(!IsSafe(cmdstr)) return "UnSafe";
        FILE* fp = popen(cmdstr.c_str(), "r");
        if(fp == nullptr)
        {
            LOG(LogLevel::ERROR) << "exec error: " << cmdstr;
            return "error";
        }

        std::string result;
        char buffer[512];
        while(fgets(buffer, sizeof(buffer), fp) != nullptr)
        {
            result += buffer;
            buffer[0] = 0; // 清空一下
        }
        pclose(fp);

        return result;
    }
    ~ExcuteCommand()
    {}
private:
    std::vector<std::string> _white_list;
};
#endif