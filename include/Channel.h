#pragma once
#include <sys/epoll.h>
#include "Epoll.h"

class Epoll;    // 前置声明Epoll类

class Channel
{
private:
    int fd_ = -1;           // Channel拥有的fd，Channel和fd一一对应
    Epoll* ep_ = nullptr;   // Channel对应的红黑树，Channel与epoll是多对一的关系，一个Channel只对应一个epoll
    bool inepoll_ = false;  // Channel是否已添加到epoll树上，如果没有，则调用epoll_ctl()的时候使用EPOLL_CTL_ADD，否则调用EPOLL_CTL_MOD
    uint32_t events_ = 0;   // fd_需要监视的事件, listenfd和clientfd需要监视EPOLLIN，clientfd可能还需要监视EPOLLOUT
    uint32_t revents_ = 0;  // fd_已发生的事件

public:
    Channel(Epoll* ep, int fd);
    ~Channel();
    int fd();
    void useet();   // 采用边缘触发
    void enablereading();   // 让epoll_wait()监视fd_的读事件
    void setinepoll();  // 把inepoll_成员的值设置为true
    void setrevents(uint32_t ev);   // 设置revents_成员的参数为ev
    bool inpoll();  // 返回inepoll_成员
    uint32_t events();
    uint32_t revents();
};
