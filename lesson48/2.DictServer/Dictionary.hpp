#ifndef __DICTIONARY__HPP
#define __DICTIONARY__HPP

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include "logger.hpp"
using namespace LogModule;

const std::string gdefaultfilename = "./Dict.txt";
const std::string gsep = ": ";
class Dictionary 
{
private:
    void LoadConfig()
    {
        std::ifstream in(_dictfilename);
        if(!in.is_open())
        {
            LOG(LogLevel::FATAL) << "open fail";
            exit(1);
        }
        LOG(LogLevel::INFO) << "open success";

        std::string line;
        while(std::getline(in, line))
        {
            // apple: 苹果 - I eat an apple every day. / 我每天吃一个苹果。
            auto pos = line.find(gsep);
            if(pos == std::string::npos) // 没有找到
            {
                LOG(LogLevel::WARNING) << "load fail";
                continue;
            }
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + gsep.size());
            _dictmp.insert({key, value});
        }
        in.close();
    }
public:
    Dictionary(const std::string dictfilename = gdefaultfilename)
        : _dictfilename(dictfilename)
    {
        LoadConfig();
    }
    std::string TransTrate(const std::string &word)
    {
        auto it = _dictmp.find(word);
        if(it == _dictmp.end())
        {
            return "未知";
        }
        return it->second;
    }
    ~Dictionary()
    {

    }
private:
    std::string _dictfilename;
    std::unordered_map<std::string, std::string> _dictmp;
};
#endif