#ifndef __PROTOCOL__HPP
#define __PROTOCOL__HPP

#include <functional>
#include <iostream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <string>
#include <jsoncpp/json/json.h>
#include <variant>
#include "Logger.hpp"

using namespace LogModule;

// 请求报文: client -> server
class Request
{
public:
    Request() {}
    Request(int x, int y, char oper): _x(x), _y(y), _oper(oper)
    {}
    void Serialize(std::string *outstring)
    {
        // 序列化: 结构化属性 多变一
        Json::Value root;
        root["datax"] = _x;
        root["datay"] = _y;
        root["oper"] = _oper;

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
        else 
        {
            std::cout << "Json, bug!" << std::endl;
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
public:
    int _x;
    int _y;
    char _oper;
};

// 应答报文: server -> client
class Response
{
public:
    Response(): _result(0), _exitcode(0)
    {}
    void Serialize(std::string *outstring)
    {
        // 序列化
        Json::Value root;
        root["result"] = _result;
        root["exitcode"] = _exitcode;

        Json::FastWriter writer;
        *outstring = writer.write(root);
    }
    void Deserialize(std::string &instring)
    {
        // 反序列化
        Json::Value root;
        Json::Reader reader;
        if(reader.parse(instring, root))
        {
            _result = root["result"].asInt();
            _exitcode = root["exitcode"].asInt();
        }
        else 
        {
            std::cout << "Json, bug!" << std::endl;
        }
    }
    ~Response()
    {}
public:
    int _result;
    int _exitcode; // 0: 结果可信 1/2/3/4...: 结果不可信，错误原因，状态码
};
 // int > 0: 提取了一个完整json报文
// int == 0: 不完整：什么都不做
// int < 0: UnPackage出错了
// "LE
// "LEN"
// "LEN"\r
// "LEN"\r\n
// "LEN"\r\n{"datax"
// "LEN"\r\n{"datax":10,"dat
// "LEN"\r\n{"datax":10,"datay":20,"oper":
// "LEN"\r\n{"datax":10,"datay":20,"oper":43}\r\n"LEN"\r\n{"datax":10,"datay":20,"oper":43}\r\n"LEN"\r\n{"datax":10,"datay":20,"oper":43}\r\n
// "LEN"\r\n{"datax":10,"datay":20,"oper":43}\r\n"LEN"\r\n{"datax"

const std::string gsep = "\r\n";
using callback_t = std::function<Response(const Request&)>;

class Protocol
{
public:
    Protocol(){}
    Protocol(callback_t cb): _cb(cb)
    {}
    ~Protocol(){}

    // 封包 {"datax":10,"datay":20,"oper":43} -> "40"\r\n{"datax":10,"datay":20,"oper":43}\r\n
    std::string Pack(const std::string &jsonstr)
    {
        uint32_t len = jsonstr.size();
        // 报文长度 + 特殊分隔符 + jsonstring + 特殊分隔符
        return std::to_string(len) + gsep + jsonstr + gsep;
    }
    int UnPack(std::string &streamstr, std::string *jsonstr)
    {
        // 1. 先去找分隔符
        auto pos = streamstr.find(gsep); 
        if(pos == std::string::npos)
            return 0; // 报文不完整,返回-1

        // 2. 走到这里证明能找到分隔符
        std::string packlenstr = streamstr.substr(0, pos); // 包装长度字符
        // packlenstr -> '0'-'9' -> 检查一下合法性
        for (auto &c : packlenstr)
        {
            if (c < '0' || c > '9')
            {
                return -1;
            }
        }
        // 如果是合法的 -> 我们再转成整数
        uint32_t packlenint = std::stoi(packlenstr); // "40"->40

        // 3. 完整的报文应该是多长,现在就可以知道了
        uint32_t targetlen = packlenstr.size() + packlenint + 2 * gsep.size();
        if(streamstr.size() < targetlen)
            return 0; // 报文不完整

        // 4. 走到这里的话, 我们就可以去返回了
        *jsonstr = streamstr.substr(pos + gsep.size(), packlenint); // 拿到了去除长度和分隔符后的实际字符串
        streamstr.erase(0, targetlen); // 记得删除这次处理完的, 所以我们传参是用的引用
        return targetlen; // 目标长度返回就行
    }
public:
    std::string HandlerRequest(std::string &streamstr)
    {
        LOG(LogLevel::DEBUG) << "Enter HandlerRequest";
        std::string resp_package; // 应答 -> 我们在外面声明, 也是为了多个报文
        // 1. 检查报文完整性, 这里使用while循环是为了处理多个报文的情况
        while(true)
        {
            // 解包
            std::string jsonstring; // 利用输出型参数去拿解包后去除长度和分隔符后的实际字符串
            int n = UnPack(streamstr, &jsonstring);
            if (n == 0)
            {
                LOG(LogLevel::DEBUG) << "解析完毕";
                // 这里我们直接返回的话? 要是上面我们收到的报文几条连在一起,但有的只有一半呢？
                // 其实是不会发生这种情况的, 在解包UnPack处理的时候, 如果我们是不完整的, 下次根本走不到下面的 +=
                // 直接在这里就已经返回了,此时的还是上次的 resp_package
                return resp_package;
            }
            else if(n == -1)
            {
                LOG(LogLevel::DEBUG) << "协议解析失败";
                exit(1);
            }
            // 如果可以走到这里, 我就保证了至少是有一个完整报文的
            LOG(LogLevel::DEBUG) << "request : jsonstring: " << jsonstring;
            
            // 2. {"datax":10,"datay":20,"oper":43} -> 反序列化(请求报文) -> 方便我们操作报文 
            Request request;
            request.Deserialize(jsonstring);

            // 3. 业务处理
            Response response;
            response = _cb(request);

            // 4. 序列化(应答报文) -> 业务处理完之后
            std::string respjsonstr;
            response.Serialize(&respjsonstr);

            LOG(LogLevel::DEBUG) << "response : respjsonstr: " << respjsonstr;

            // 5. 封包 -> 可以回复给客户端
            resp_package += Pack(respjsonstr); // 这里采用 +=
            LOG(LogLevel::DEBUG) << "Pack: \r\n" << resp_package;
        }
        return resp_package;
    }
private:
    callback_t _cb;
};



// 问题：client -> request -> {"datax":10,"datay":20,"oper":43} -> write -> OS -> 网络 -> 收到
// 问题：粘包问题
// {"datax"
// {"datax":10,"dat
// {"datax":10,"datay":20,"oper":
// {"datax":10,"datay":20,"oper":43}
// {"datax":10,"datay":20,"oper":43}{"datax"
// {"datax":10,"datay":20,"oper":43}{"datax":10,"datay":20,"oper":43}{"datax"s

// "LEN"\r\njsonstring -> 你怎么保证报文是完整的?
// "LE
// "LEN"
// "LEN"\r\
// "LEN"\r\n
// "LEN"\r\n{"datax"
// "LEN"\r\n{"datax":10,"dat
// "LEN"\r\n{"datax":10,"datay":20,"oper":
// "LEN"\r\n{"datax":10,"datay":20,"oper":43}
// "LEN"\r\n{"datax":10,"datay":20,"oper":43}"LEN"\r\n{"datax"
// "LEN"\r\n{"datax":10,"datay":20,"oper":43}"LEN"\r\n{"datax":10,"datay":20,"oper":43}"LEN"\r\n{"datax"s

// 报文长度 + 特殊分隔符 + jsonstring + 特殊分隔符
// "LEN"\r\njsonstring\r\n


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
