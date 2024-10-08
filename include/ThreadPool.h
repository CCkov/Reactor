#pragma once
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unistd.h>
#include <sys/syscall.h> 

class ThreadPool
{
private:
    std::vector<std::thread> threads_;              // 线程池中的线程
    std::queue<std::function<void()>> taskqueue_;   // 任务队列
    std::mutex mutex_;                              // 任务队列同步的互斥锁
    std::condition_variable condition_;             // 任务队列同步的条件变量
    std::atomic_bool stop_;                         // 在析构函数中，把stop_的值设为true，全部的线程将退出  

    std::string threadtype_;    // 线程种类:IO 和 WORKS
public:
    ThreadPool(size_t threadnum, const std::string& threadtype);
    ~ThreadPool();
    void addtask(std::function<void()> task);
    size_t size();  // 获取线程池大小
    void stop();
};
