#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <functional>
#include <unistd.h>
#include <sys/syscall.h>

class Epoll;
class Channel;
class Eventloop
{
private:
    Epoll* ep_; // 每个事件循环只有一个epoll
    std::function<void(Eventloop*)> epolltimeoutcallback_;
public:
    Eventloop();    // 在构造函数中创建epoll对象ep_
    ~Eventloop();   // 在析构函数中销毁ep_

    void run(); // 运行事件循环
    Epoll* ep();

    void updatechannel(Channel* ch);
    void setepolltimeoutcallback(std::function<void(Eventloop*)> fn);

    void removechannel(Channel* ch);
};


