#pragma once

#include <iostream>
#include <string>

class Task
{
public:
    Task(const std::string &who, int x, int y):_x(x), _y(y), _who(who)
    {}
    Task()
    {}
    void Execute()
    {
        _result = _x + _y;
    }
    std::string GetResult()
    {
        return std::to_string(_x) + " + " + std::to_string(_y) + " = " + std::to_string(_result);
    }
    ~Task()
    {}
private:
    int _x;
    int _y;
    int _result;
    std::string _who;
};

// class Task
// {
// public:
//     Task(const std::string &who, int x, int y):_x(x), _y(y), _who(who)
//     {}
//     Task()
//     {}
//     void operator()()
//     {
//         std::cout << _who << " execute task: " <<_x << " + " << _y << " = " << _x + _y << std::endl;
//     }
//     ~Task()
//     {}
// private:
//     int _x;
//     int _y;
//     std::string _who;
// };
