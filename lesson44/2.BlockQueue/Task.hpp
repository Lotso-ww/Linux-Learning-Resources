#ifndef TASK_HPP
#define TASK_HPP

#include <iostream>
#include <functional>
#include <string>

class Task
{
public:
    using func_t = std::function<int(int, int, char)>;

    Task() : _a(0), _b(0), _op('+'), _result(0) {}
    Task(int a, int b, char op) : _a(a), _b(b), _op(op), _result(0) {}

    void Run()
    {
        switch (_op)
        {
        case '+': _result = _a + _b; break;
        case '-': _result = _a - _b; break;
        case '*': _result = _a * _b; break;
        case '/': _result = _b != 0 ? _a / _b : 0; break;
        default: _result = 0; break;
        }
    }
    std::string GetResult()
    {
        return std::to_string(_result);
    }
    std::string GetTaskRes()
    {
        Run();
        return std::to_string(_a) + " " + std::string(1, _op) + " " + 
               std::to_string(_b) + " = ";
    }

    int GetResult() const { return _result; }

private:
    int _a;
    int _b;
    char _op;
    int _result;
};

#endif
