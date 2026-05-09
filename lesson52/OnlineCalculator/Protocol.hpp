#ifndef __PROTOCOL__HPP
#define __PROTOCOL__HPP

#include <iostream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <string>
#include <jsoncpp/json/json.h>

// 请求报文: client -> server
class Request
{
public:
    Request(int x, int y, char oper): _x(x), _y(y), _oper(oper)
    {}
    void Serialize(std::string *outstring)
    {
        // 序列化: 结构化属性 多变一
        Json::Value root;
        root["datax"] = _x;
        root["datay"] = _y;
        root["oper"] = _oper;

        root["score"].append(10);
        root["score"].append(20);
        root["score"].append(30);
        root["score"].append(40);
        root["score"].append(50);

        Json::FastWriter writer;
        *outstring = writer.write(root);
    }
    void Deserialize(std::string &instring)
    {
        // 反序列化: 字节流属性, 一变多
        Json::Value root;
        Json::Reader reader;
        if(reader.parse(instring, root))
        {
            _x = root["datax"].asInt();
            _y = root["datay"].asInt();
            _oper = root["oper"].asInt();
        }
    }

    void Print()
    {
        std::cout << "_x: " << _x << std::endl;
        std::cout << "_y: " << _y << std::endl;
        std::cout << "_oper: " << _oper << std::endl;
    }
    ~Request()
    {}
private:
    int _x;
    int _y;
    char _oper;
};

// 应答报文: server -> client
class Response
{
public:
    Response()
    {}
    void Serialize()
    {
        // 序列化
    }
    void Deserialize()
    {
        // 反序列化
    }
    ~Response()
    {}
private:
    int _result;
    int _exitcode; // 0: 结果可信 1/2/3/4...: 结果不可信，错误原因，状态码
};

// 如何序列化？如何反序列化？
// Request 为例，自己做序列化和反序列化
// 今天先说方案1：
// "_x _oper _y" -- 序列化
// 如何解决粘包问题？是应用层自己解决的！
// len -> "len"\r\n"_x _oper _y"\r\n
// 解包
// 得到完整报文
// 反序列化
// 方案2：json方案
#endif
