#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <functional>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>

class Epoll;
class Channel;
class Eventloop
{
private:
    std::unique_ptr<Epoll> ep_; // 每个事件循环只有一个epoll
    std::function<void(Eventloop*)> epolltimeoutcallback_;
    pid_t threadid_;    // 事件循环所在线程的id
    std::queue<std::function<void()>> taskqueue_;   // 事件循环线程被eventfd唤醒后执行的任务队列
    std::mutex mutex_;  // 任务队列同步的互斥锁
    int wakeupfd_;  // 用于事件循环线程的eventfd
    std::unique_ptr<Channel> wakechannel_;  // eventfd的Channel
public:
    Eventloop();    // 在构造函数中创建epoll对象ep_
    ~Eventloop();   // 在析构函数中销毁ep_

    void run(); // 运行事件循环
    // Epoll* ep();

    void updatechannel(Channel* ch);
    void setepolltimeoutcallback(std::function<void(Eventloop*)> fn);

    void removechannel(Channel* ch);

    bool isinloopthread();  // 判断当前线程是否在事件循环线程中

    void queueinloop(std::function<void()> fn);   // 把任务添加到任务队列中
    void wakeup();  // 用eventfd唤醒事件循环线程
    void handlewakeup();    // 事件循环线程被eventfd唤醒后执行的函数
};


