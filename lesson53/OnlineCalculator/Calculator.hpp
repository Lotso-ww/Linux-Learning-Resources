#ifndef __CALCULATOR__HPP
#define __CALCULATOR__HPP

#include <iostream>
#include <string>
#include "Protocol.hpp"
#include "Logger.hpp"

using namespace LogModule;

class Calculator
{
public:
    Calculator(){}
    ~Calculator(){}
public:
    Response Exec(const Request& req)
    {
        LOG(LogLevel::DEBUG) << "Enter Calculator Exec";
        
        Response resp; // {0, 0}
        switch (req._oper) 
        {
        case '+':
            resp._result = req._x + req._y;
            break;
        case '-':
            resp._result = req._x - req._y;
            break;
        case '*':
            resp._result = req._x * req._y;
            break;
        case '/':
        {
            if(req._y == 0)
                resp._exitcode = -1; // div 0
            else
                resp._result = req._x / req._y;
        }
        break;
        case '%':
        {
            if(req._y == 0)
                resp._exitcode = -2; // mod 0
            else
                resp._result = req._x % req._y;
        }
        break;
        default:
            resp._exitcode = 3; // oper code error
            break;
        }

        return resp;
    }
};
#endif