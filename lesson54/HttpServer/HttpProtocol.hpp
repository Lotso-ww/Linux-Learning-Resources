#ifndef __HTTPPROTOCOL__HPP
#define __HTTPPROTOCOL__HPP
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include "Logger.hpp"

using namespace LogModule;

const std::string linesep = "\r\n";
const std::string headeresp = ": ";
const std::string webroot = "wwwroot";

class HttpRequest
{
private:
    int ReadOneLine(std::string &streamstr, std::string *line)
    {
        // 我们先去找 linesep
        auto pos = streamstr.find(linesep);
        if(pos == std::string::npos)
            return -1; // 出现了问题
        // 走到这里就读到了一行 -- 我们需要处理一下
        // a. 先拿到这行
        *line = streamstr.substr(0, pos);
        // b. 删掉原始字符串中的这一行, 加上分隔符号
        streamstr = streamstr.erase(0, pos + linesep.size());

        // n == 0 证明读到了空行
        return line->size();
    }

    void ParseLine(std::string &request_line)
    {
        std::stringstream ss(request_line);
        ss >> _method >> _uri >> _http_version;

        _path = webroot + _uri;
    }

    void SplitString(std::string &line, std::string *key, std::string *value, const std::string sep = headeresp)
    {
        // a. 先找分隔符号
        auto pos = line.find(sep);
        if(pos == std::string::npos)
            return;
        *key = line.substr(0, pos);
        *value = line.substr(pos + sep.size());
    }

    void PrintDebug()
    {
        // 打印进行观察
        std::cout << "reqline:" << _method << "#" << _uri << "#" << _http_version << std::endl;\
        for(auto &item : _request_headerkv)
        {
            std::cout << item.first << "->" << item.second << std::endl;
        }

        std::cout << "blankline: "<< _blankline << std::endl;
        std::cout << "body: " << _body << std::endl;
    }
public:
    HttpRequest(){}
    void Deserialize(std::string &streamstr)
    {
        // 1. 我们先读取第一行 -- 请求行
        std::string request_line;
        // streamstr给的引用, 是因为里面要找到一行就删掉这一行
        // request_line给的是指针, 我们可以通过这个拿到读取到的
        int n = ReadOneLine(streamstr, &request_line); 
        // 可以进行简单的判断,我这里就不处理了
        (void)n;

        // 2. 解析请求行
        ParseLine(request_line);

        // 3. 继续去解析其他行
        n = 0;
        do 
        {
            std::string line;
            n = ReadOneLine(streamstr, &line);
            if(n > 0)
            {
                // 这里就需要处理
                std::string key, value;
                SplitString(line, &key, &value, headeresp);
                // 这里判断一下
                if(!key.empty() && !value.empty())
                {
                    _request_headerkv[key] = value; // 存进去
                }
            }
            else if(n < 0)
            {
               LOG(LogLevel::DEBUG) << "ReadOneLine, bug?";
               break; 
            }
            else
            {
                _blankline = "\r\n";
                break;
            }
        }while (n > 0);

         if(_request_headerkv.find("Content-Length") != _request_headerkv.end())
        {
            int len = std::stoi(_request_headerkv["Content-Length"]); // 去哈希表里面找这个属性看看有没有 -- 判断有没有正文
            _body = streamstr.substr(0, len); // 可以这样找, 是因为前面我们都是找一行删一行
            // _body = streamstr;
        }

        PrintDebug();
    }
    // 方便外部可以获取我请求里面的东西
    std::string operator[](const std::string& key) const
    {
        if(key == "method")
            return _method;
        else if(key == "uri")
            return _uri;
        else if(key == "httpversion")
            return _http_version;
        else if(key == "body")
            return _body;
        else if(key == "path")
            return _path;
        else
        {
            auto iter = _request_headerkv.find(key);
            if(iter != _request_headerkv.end())
                return iter->second;
        }

        // 再就是直接返回空串了
        return std::string();
    }
    ~HttpRequest(){}
private:
    std::string _method;
    std::string _uri; // URL -> a/b/c/index.html
    std::string _http_version;
    std::unordered_map<std::string,std::string> _request_headerkv;
    std::string _blankline;
    std::string _body;
private:
    std::string _path;
};

class HttpResponse
{
public:
    HttpResponse(){}
    void Serialize(std::string *outstr)
    {

    }
    ~HttpResponse(){}
private:
    std::string _http_version;
    std::string _status_code;
    std::string _status_code_desc;
    std::unordered_map<std::string,std::string> _response_headerkv;
    std::string _blankline;
    std::string _body;
};
#endif