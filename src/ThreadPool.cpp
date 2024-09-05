#include "../include/ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum, const std::string& threadtype)
    :stop_(false), threadtype_(threadtype)
{
    for (size_t i = 0; i < threadnum; i++)
    {
        // 用lambda函数创建线程
        threads_.emplace_back([this]
        {
            printf("create %s thread is %ld\n",threadtype_.c_str(), syscall(SYS_gettid));

            while (stop_ == false)
            {
                std::function<void()> task;
                
                {   // 锁作用的域开始
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    // 等待生产者的条件变量
                    this->condition_.wait(lock, [this]
                    {
                        return ((this->stop_ == true) || (this->taskqueue_.empty() == false));
                    });
                    
                    // 在线程池停止之前，如果队列中还有任务，执行完再退出
                    if ((this->stop_ == true) && (this->taskqueue_.empty() == true)) return;
                    // 出队一个任务
                    task = std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                    
                    // 锁作用域结束
                }
                printf("%s(%ld) execute task\n",threadtype_.c_str() ,syscall(SYS_gettid));
                task(); // 执行任务
            }
            
        });
    }
    
}

ThreadPool::~ThreadPool()
{
    stop_ = true;
    condition_.notify_all();
    for (std::thread &i : threads_)
    {
        i.join();
    }
    
}

void ThreadPool::addtask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }

    condition_.notify_one();
}
