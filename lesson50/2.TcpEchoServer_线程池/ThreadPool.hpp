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

/**
 * @brief 线程池单例模板类
 * 采用了“懒汉模式”实现，即在第一次调用 GetInstance 时才进行实例化。
 */
template<typename T>
class ThreadPool 
{
private: 
    // 内部逻辑：判定队列状态
    bool IsEmptyQueue()
    {
        return _queue.empty();
    }
    
    // 内部逻辑：原子化提取任务（调用前需持有锁）
    T PopHelper()
    {
        T t = _queue.front();
        _queue.pop();
        return t;
    }

    /**
     * @brief 消费者核心执行流
     * 运行于子线程栈中，通过条件变量实现高效的任务等待与唤醒。
     */
    void ThreadRoutine()
    {
        char name[64];
        pthread_getname_np(pthread_self(), name, sizeof(name));
        while(true)
        {
            T task; // 任务对象
            // 临界区作用域：确保锁的持有时间最短化
            {
                LockGuard lockGuard(&_mutex); // 加锁保护
                
                // 1. 任务队列为空 && 线程处于运行状态(不退出) -- 允许休眠
                /**
                 * 深度解析：
                 * 此处的 while 循环不仅解决了“虚假唤醒”，还配合单例模式
                 * 确保了多个子线程在竞争唯一任务队列时的逻辑严密性。
                 */
                while(IsEmptyQueue() && _isrunning) // 防止伪唤醒
                {
                    LOG(LogLevel::DEBUG) << "没有任务,线程休眠: " << "|" << name << "|";
                    _sleeper_cnt++;
                    _cond.Wait(_mutex); // 核心：释放锁 -> 挂起 -> 被唤醒 -> 重获锁
                    _sleeper_cnt--;
                    LOG(LogLevel::DEBUG) << "有任务,线程唤醒: " << "|" << name << "|";
                }

                // 2. 任务队列为空 && 线程不处于运行状态(要退出) -- 允许退出
                if(IsEmptyQueue() && !_isrunning)
                {
                    LOG(LogLevel::INFO) << "Thread: " << name << "quit";
                    break; 
                }

                // 3. 任务队列不为空，无论运行状态如何，都要提取任务处理
                task = PopHelper();
            }
            // 任务执行放在锁外，这是实现真正并发、避免线程池退化为单线程的关键
            task(); 
        }
    }

// 单例模式防御：私有化构造函数，杜绝外部随意创建对象
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
    /**
     * 补充建议：
     * 虽然这里用了 = default，但在标准的单例模式中，
     * 拷贝构造和赋值运算符通常应该设为 = delete，以防止实例被“克隆”。
     */
    ThreadPool(const ThreadPool<T>& ) = delete;
    ThreadPool<T>& operator =(const ThreadPool<T>&) = delete;

public:
    // 定义成静态的：全局唯一访问点
    /**
     * @brief 获取单例对象的静态接口
     * 采用了“双检查锁 (Double-Checked Locking)”机制。
     */
    static ThreadPool<T>* GetInstance()
    {
        // 第一层判断：为了提高性能。如果实例已存在，直接返回，避免不必要的加锁开销。
        if(_instance ==  nullptr)
        {
            // 加锁：保证创建实例过程的原子性，防止多个线程同时执行 new 操作
            LockGuard lockGuard(&_signalton_lock);
            
            // 第二层判断：为了保证唯一性。在获得锁后再次检查，
            // 确认在此期间没有其他线程提前创建了实例。
            if(_instance == nullptr)
            {
                LOG(LogLevel::DEBUG) << "首次创建,创建成功" ;
                _instance = new ThreadPool<T>(); // 只会创建一次
                _instance->Start(); // 创建出来运行一次

            }
        }
        return _instance;
    }

    // 启动线程服务
    void Start()
    {
        LockGuard lockGuard(&_mutex);
        if(_isrunning)
            return;
        _isrunning = true;
        for(auto& thread: _threads)
            thread.start();
    }

    // 生产者下发任务
    void Enqueue(const T& task)
    {
        LockGuard lockGuard(&_mutex);
        if(!_isrunning) // 如果当前处于停止状态,禁止继续加任务
            return;
        _queue.push(task);
        
        // 唤醒一个来线程来执行任务
        // 优化策略：只有存在正在睡觉的工人才发通知
        if(_sleeper_cnt > 0)
            _cond.NotifyOne();
    }

    // 优雅停止线程池
    void Stop()
    {
        LockGuard lockGuard(&_mutex);
        if(_isrunning)
        {
            LOG(LogLevel::DEBUG) << "关闭线程池";
            _isrunning = false; // 改变状态，作为 ThreadRoutine 退出的触发信号
            
            // 唤醒所有的去执行, 保证所有线程都能意识到状态改变并正确 break
            if(_sleeper_cnt > 0)
                _cond.NotifyAll();
        }
    }

    // 阻塞式资源回收
    void Wait()
    {
        // join 操作本身阻塞，且不涉及临界资源修改，故无需加锁
        for(auto& thread: _threads)
            thread.join();
    }

    ~ThreadPool()
    {}

private:
    // 线程池管理组件
    std::vector<Thread> _threads; // 管理线程对象的容器
    int _num;                    // 线程池规模
    bool _isrunning;             // 全局生命周期开关
    int _sleeper_cnt;            // 记录当前空闲工人的数量

    std::queue<T> _queue;        // 共享任务队列
    Mutex _mutex;                // 保护任务队列的互斥锁
    Cond _cond;                  // 协调生产/消费节奏的条件变量

    // 单例模式静态成员
    static ThreadPool<T> *_instance;    // 全局唯一实例指针
    static Mutex _signalton_lock;       // 保护单例实例化的静态锁
};

// 静态成员变量在类外初始化：
// 静态指针在 main 运行前初始化为 null，保证 GetInstance 的逻辑起点正确。
template<typename T>
ThreadPool<T>* ThreadPool<T>::_instance = nullptr;

template <typename T>
Mutex ThreadPool<T>::_signalton_lock;

#endif
