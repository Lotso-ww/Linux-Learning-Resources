#ifndef __HTTPPROTOCOL__HPP
#define __HTTPPROTOCOL__HPP
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include "Logger.hpp"

using namespace LogModule;

const std::string linesep = "\r\n";
const std::string headersep = ": ";
const std::string webroot = "wwwroot";
const std::string homepage = "index.html";
const std::string gdefaulthttpversion = "HTTP/1.0";
const std::string gspace = " ";
const std::string suffixsep = ".";
const std::string argsep = "?";


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

        if(_uri == "/")
        {
            _path = webroot + _uri + homepage;
        }
        else
        {
            _path = webroot + _uri;
        }

        // /content.html?username=zhangsan&passwd=123456
        if(_method == "GET" || _method == "get")
        {
            auto pos = _path.find(argsep);
            if(pos != std::string::npos)
            {
                _args = _path.substr(pos + argsep.size());
                _path = _path.substr(0, pos);
            }
        }

        // 解析后缀
        // 分析请求的资源的后缀！ // wwwroot + / + index.html wwwroot/image/th.jpg wwwroot/css/XXX.css
        auto pos = _path.rfind(suffixsep);
        if(pos == std::string::npos)
        {
            _suffix = ".html";
        }
        else 
        {
            _suffix = _path.substr(pos); // .css .jpg ....
        }

        std::cout << "=======: PATH: " << _path << std::endl;
    }

    void SplitString(std::string &line, std::string *key, std::string *value, const std::string sep = headersep)
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
        std::cout << "reqline:" << _method << "#" << _uri << "#" << _http_version << std::endl;
        for (auto &item : _request_headerkv)
        {
            std::cout << item.first << "->" << item.second << std::endl;
        }

        std::cout << "blankline: " << _blankline << std::endl;
        std::cout << "body: " << _body << std::endl;
        std::cout << "suffix: " << _suffix << std::endl;
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
                SplitString(line, &key, &value, headersep);
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
            if(_method == "POST" || _method == "post")
            {
                _args = _body;
            }
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
        else if(key == "suffix")
            return _suffix;
        else if(key == "args")
            return _args;
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
    std::string _suffix; // 后缀
    std::string _args; // 提参
};

class HttpResponse
{
public:
    HttpResponse(): _http_version(gdefaulthttpversion), _blankline(linesep)
    {}

    void Serialize(std::string *outstr)
    {
        std::string status_line = _http_version + gspace + std::to_string(_status_code) + gspace + _status_code_desc + linesep;
        std::string response_header;
        for(auto& it: _response_headerkv)
        {
            std::string header = it.first + headersep + it.second + linesep;
            response_header += header;
        }
        *outstr = status_line + response_header + _blankline + _body;
    }
    void SetBody(const std::string &content)
    {
        _body = content;
    }
    void SetCode(int code)
    {
        _status_code = code;
        _status_code_desc = Code2Desc(code);
    }

    void SetHeader(const std::string &key, const std::string &value)
    {
        _response_headerkv[key] = value;
    }
    void SetHeader(const std::string &key, int value)
    {
        _response_headerkv[key] = std::to_string(value);
    }
    ~HttpResponse(){}
private:
    std::string Code2Desc(int code)
    {
        switch (code)
        {
        // 1xx: 信息响应
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocols";
        case 102:
            return "Processing"; // WebDAV
        case 103:
            return "Early Hints";

        // 2xx: 成功
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 207:
            return "Multi-Status"; // WebDAV
        case 208:
            return "Already Reported";
        case 226:
            return "IM Used";

        // 3xx: 重定向
        case 300:
            return "Multiple Choices";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 305:
            return "Use Proxy";
        case 306:
            return "Switch Proxy"; // 已废弃，但仍保留
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";

        // 4xx: 客户端错误
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 402:
            return "Payment Required";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot"; // 愚人节笑话，但常被实现
        case 421:
            return "Misdirected Request";
        case 422:
            return "Unprocessable Entity"; // WebDAV
        case 423:
            return "Locked";
        case 424:
            return "Failed Dependency";
        case 425:
            return "Too Early";
        case 426:
            return "Upgrade Required";
        case 428:
            return "Precondition Required";
        case 429:
            return "Too Many Requests";
        case 431:
            return "Request Header Fields Too Large";
        case 451:
            return "Unavailable For Legal Reasons";

        // 5xx: 服务端错误
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";
        case 506:
            return "Variant Also Negotiates";
        case 507:
            return "Insufficient Storage"; // WebDAV
        case 508:
            return "Loop Detected";
        case 510:
            return "Not Extended";
        case 511:
            return "Network Authentication Required";

        // 未知状态码
        default:
            return "Unknown";
        }
    }
private:
    std::string _http_version;
    int _status_code;
    std::string _status_code_desc;
    std::unordered_map<std::string,std::string> _response_headerkv;
    std::string _blankline;
    std::string _body;
};
#endif