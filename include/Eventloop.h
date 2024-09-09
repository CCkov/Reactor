#pragma once
#include "Epoll.h"
#include "Channel.h"
#include "Timestamp.h"
#include "Connection.h"
#include <functional>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <map>
#include <atomic>

class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;

class Eventloop
{
private:
    int timetvl_;
    int timeout_;
    std::unique_ptr<Epoll> ep_; // 每个事件循环只有一个epoll
    std::function<void(Eventloop*)> epolltimeoutcallback_;
    pid_t threadid_;    // 事件循环所在线程的id
    std::queue<std::function<void()>> taskqueue_;   // 事件循环线程被eventfd唤醒后执行的任务队列
    std::mutex mutex_;  // 任务队列同步的互斥锁
    int wakeupfd_;  // 用于事件循环线程的eventfd
    std::unique_ptr<Channel> wakechannel_;  // eventfd的Channel
    int timerfd_;   //定时器的fd
    std::unique_ptr<Channel> timerchannel_;  // eventfd的Channel
    bool mainloop_; // true表示主事件循环，false表示从事件循环
    std::mutex mmutex_; // 保护conns_的互斥锁
    std::map<int, spConnection> conns_; // 存放运行在该事件循环上的全部Connection对象

    std::function<void(int)> timercallback_;    // 删除TcpServer中超时的Connection对象，将被设置为TcpServer::removconn()
    std::atomic_bool stop_;
public:
    Eventloop(bool mainloop, int timetvl=30, int timeout=80);    // 在构造函数中创建epoll对象ep_
    ~Eventloop();   // 在析构函数中销毁ep_

    void run(); // 运行事件循环
    // Epoll* ep();
    void stop();

    void updatechannel(Channel* ch);
    void setepolltimeoutcallback(std::function<void(Eventloop*)> fn);

    void removechannel(Channel* ch);

    bool isinloopthread();  // 判断当前线程是否在事件循环线程中

    void queueinloop(std::function<void()> fn);   // 把任务添加到任务队列中
    void wakeup();  // 用eventfd唤醒事件循环线程
    void handlewakeup();    // 事件循环线程被eventfd唤醒后执行的函数

    int createtimerfd(int sec);
    void handletimer(); // 闹钟响时执行的函数

    void newconnection(spConnection conn);  // 把Connection对象保存在conns_ 中

    void settimercallback(std::function<void(int)> fn);
};


