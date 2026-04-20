#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

// 可以看到我们直接使用了很多之前自己造的轮子
#include <iostream>
#include <memory>
#include <pthread.h>
#include <vector>
#include <queue>
#include "Thread.hpp"
#include "Logger.hpp"
#include "Mutex.hpp"
#include "Cond.hpp"
using namespace LogModule;

const static int gDefaultCnt = 5;
// #if 0
// void DefaultRun()
// {
//     char name[64];
//     pthread_getname_np(pthread_self(), name, sizeof(name));
//     while(true)
//     {
//         LOG(LogLevel::DEBUG) << name << "线程执行默认方法";
//         sleep(1);
//     }
// }
// #endif

template<typename T>
class ThreadPool 
{
private: 
    bool IsEmptyQueue()
    {
        return _queue.empty();
    }
    T PopHelper()
    {
        T t = _queue.front();
        _queue.pop();
        return t;
    }
    void ThreadRoutine()
    {
        char name[64];
        pthread_getname_np(pthread_self(), name, sizeof(name));
        while(true)
        {
            T task; // 任务对象
            // 临界区
            {
                LockGuard lockGuard(&_mutex); // 加锁保护
                // 1. 任务队列为空 && 线程处于运行状态(不退出) -- 允许休眠
                while(IsEmptyQueue() && _isrunning) // 防止伪唤醒
                {
                    LOG(LogLevel::DEBUG) << "没有任务,线程休眠: " << "|" << name << "|";
                    _sleeper_cnt++;
                    _cond.Wait(_mutex);
                    _sleeper_cnt--;
                    LOG(LogLevel::DEBUG) << "有任务,线程唤醒: " << "|" << name << "|";
                }
                // 2. 任务队列为空 && 线程不处于运行状态(要退出) -- 允许退出
                if(IsEmptyQueue() && !_isrunning)
                {
                    LOG(LogLevel::INFO) << "Thread: " << name << "quit";
                    break;
                }
                // 3. 任务队列不为空 && 线程处于运行状态(不退出)   -- 要先处理完任务
                //    任务队列不为空 && 线程不处于运行状态(要退出) -- 要先处理完任务
                // 到这里了肯定是有任务
                task = PopHelper();
            }
            task(); // 任务处理放在临界区外面来
        }
    }
// 私有化
private:
    ThreadPool(int num = gDefaultCnt): _num(num), _isrunning(false), _sleeper_cnt(0)
    {
        for(int i = 0; i < num; i++)
        {
            // 利用lambda表达式捕捉this指针,不然会出现参数不匹配的问题
            // 跟Thread.hpp中有关系
            _threads.emplace_back([this](){
                this->ThreadRoutine();
            });
        }
    }

    // 将拷贝和赋值语句去掉
    ThreadPool(const ThreadPool<T>& ) = default;
    ThreadPool<T>& operator =(const ThreadPool<T>&) = default;
public:
    // 定义成静态的
    // 双if判断线程安全单例模式
    static ThreadPool<T>* GetInstance()
    {
        // 只获取一个实例
        if(_instance ==  nullptr)
        {
            LockGuard lockGuard(&_signalton_lock);
            if(_instance == nullptr)
            {
                LOG(LogLevel::DEBUG) << "首次创建,创建成功" ;
                _instance = new ThreadPool<T>(); // 只会创建一次
            }
        }
        return _instance;
    }
    void Start()
    {
        LockGuard lockGuard(&_mutex);
        if(_isrunning)
            return;
        _isrunning = true;
        for(auto& thread: _threads)
            thread.start();
    }
    void Enqueue(const T& task)
    {
        LockGuard lockGuard(&_mutex);
        if(!_isrunning) // 如果当前处于停止状态,禁止继续加任务
            return;
        _queue.push(task);
        // 唤醒一个来线程来执行任务
        if(_sleeper_cnt > 0)
            _cond.NotifyOne();
        // if(_sleeper_cnt > 0 && _queue.size() > _num)
        //     _cond.NotifyOne();
    }
    void Stop()
    {
        LockGuard lockGuard(&_mutex);
        // 处于运行状态才有停止的必要
        if(_isrunning)
        {
            LOG(LogLevel::DEBUG) << "关闭线程池";
            _isrunning = false; // 将状态改为false;
            // 唤醒所有的去执行,因为可能停止了但是任务还没做完
            if(_sleeper_cnt > 0)
                _cond.NotifyAll();
            // 这样的做法不好
            // for(auto& thread: _threads)
            //     thread.stop();
        }
    }
    void Wait()
    {
        // 这里就不加锁了,防止阻塞
        for(auto& thread: _threads)
            thread.join();
    }
    ~ThreadPool()
    {}
private:
    std::vector<Thread> _threads; // 管理线程的数组
    int _num; // 创建出来的线程数量
    bool _isrunning; // 状态标识
    // int _status; // 我们其实可以标识更多状态：RUNNING,PAUSE,STOOPED
    int _sleeper_cnt; // 正在睡眠的数量

    std::queue<T> _queue; // 任务队列
    Mutex _mutex;
    Cond _cond;

    // 单例模式
    static ThreadPool<T> *_instance;
    static Mutex _signalton_lock; 
};

// 静态成员变量在类外初始化
template<typename T>
ThreadPool<T>* ThreadPool<T>::_instance = nullptr;

template <typename T>
Mutex ThreadPool<T>::_signalton_lock;

#endif