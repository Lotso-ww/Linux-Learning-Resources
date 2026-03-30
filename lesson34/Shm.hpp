#ifndef __SHM_HPP__
#define __SHM_HPP__

#include <iostream>
#include <cstdio>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#include <io>
const std::string proj_name = "/home";
const int proj_id = 0x6666;
const int g_size = 4096;

static std::string ToHex(long long data)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "0x%llx", data);
    return buf;
}

class Shm
{
public:
    Shm(int size = g_size): _shmid(-1), _size(size), _key(0)
    {}
    ~Shm() {}
private:
    key_t GetKey()
    {
        _key = ftok(proj_name.c_str(), proj_id);
        if(_key < 0)
        {
            perror("ftok");
        }
        return _key;
    }
    bool CreateCoreHelper(int flags)
    {
        // 1. 获取key值
        key_t key = GetKey();
        // 2. 创建共享内存
        _shmid = shmget(key, _size, flags);
        if(_shmid < 0)
        {
            perror("shmget");
            return false;
        }
        return true;
    }
public:
    // 1.创建共享内存
    bool Create()
    {
        return CreateCoreHelper(IPC_CREAT | IPC_EXCL | 0666);
    }
    // 2.获取共享内存
    bool Get()
    {
        return CreateCoreHelper(IPC_CREAT);
    }
    // 3. 删除共享内存
    bool Delete()
    {
        int n = shmctl(_shmid, IPC_RMID, nullptr);
        return n < 0 ? false : true;
    }
    // 4. 获取共享内存属性
    void GetShmAttr()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid, IPC_STAT, &ds);
        if(n < 0)
        {
            perror("shmctl");
            return ;
        }
        std::cout << ds.shm_cpid << std::endl;
        std::cout << ds.shm_segsz << std::endl;
        std::cout << ToHex(_key) << std::endl;
    }
    // 5. 共享内存映射挂载
    void *Attch()
    {
        _start = (char *)shmat(_shmid, nullptr, 0);
        return _start;
    }
    // 6. 共享内存去关联
    void Detach()
    {
        int n = shmdt(_start);
        (void)n;
    }
    void Debug()
    {
        std::cout << "shmid: " << _shmid << std::endl;
        std::cout << "size: " << _size << std::endl;
        std::cout << "key: " << ToHex(_key) << std::endl;
    }
private:
    int _shmid;
    int _size;
    key_t _key;
    char *_start;
};

typedef struct data
{
    int count;
    char buf[26 * 2];
}buffer_t;
#endif
