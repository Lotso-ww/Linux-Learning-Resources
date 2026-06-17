#ifndef __CALCULATOR__HPP
#define __CALCULATOR__HPP

#include "Protocol.hpp"
#include "Logger.hpp"

using namespace LogModule;

// 业务逻辑层（Business Logic Layer）：
// 这是一个纯粹的计算组件，实现了高度的“模块解耦”。
// 它完全不感知底层的网络通信（TCP流）或数据的序列化方式（JSON格式），只负责根据标准化的 Request 对象运算并返回 Response。
class Calculator
{
public:
    Calculator(){}
    ~Calculator(){}
public:
    // 核心执行函数：输入请求对象，输出处理完毕的响应对象
    Response Exec(const Request& req)
    {
        LOG(LogLevel::DEBUG) << "Enter Calculator Exec";
        
        // 初始化响应对象。
        // 根据 Protocol.hpp 中 Response 的无参构造函数，此时 _result = 0, _exitcode = 0。
        // _exitcode 为 0 默认代表计算正常完成，结果可信。
        Response resp; // {0, 0}
        
        // 根据请求报文中的操作符（_oper）进行多路分支计算
        switch (req._oper) 
        {
        case '+':
            resp._result = req._x + req._y;
            break;
        case '-':
            // 注意操作数的严格顺序：协议隐性规定了 _x 是左操作数，_y 是右操作数
            resp._result = req._x - req._y;
            break;
        case '*':
            resp._result = req._x * req._y;
            break;
        case '/':
        {
            // 核心防御性编程（Defensive Programming）：
            // 在服务器代码中，绝对不能发生硬件级别的除 0 异常（会触发 SIGFPE 信号，直接杀死整个服务器进程）。
            // 必须在业务层进行拦截，并将非法操作转化为约定好的错误状态码（_exitcode）返回给客户端。
            if(req._y == 0)
                resp._exitcode = -1; // div 0 （错误码 -1：除0错误，此时客户端应忽略 _result 的值）
            else
                resp._result = req._x / req._y;
        }
        break;
        case '%':
        {
            // 模运算同样需要防止对 0 取模引发的服务器崩溃
            if(req._y == 0)
                resp._exitcode = -2; // mod 0 （错误码 -2：模0错误）
            else
                resp._result = req._x % req._y;
        }
        break;
        default:
            // 兜底容错处理：如果客户端传来了未定义的非法操作符（例如 '&', '|', 'a' 等），给予明确的错误码反馈
            resp._exitcode = 3; // oper code error （错误码 3：非法/不支持的操作符）
            break;
        }

        // 将填充好的应答对象（包含计算结果或错误码）返回给上层（协议层），由上层接管后续的序列化与发送逻辑
        return resp;
    }
};
#endif
