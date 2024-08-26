#pragma once
#include <sys/epoll.h>
#include <functional>
#include "Epoll.h"
#include "Socket.h"
#include "Eventloop.h"

class Eventloop;
class Channel
{
private:
    int fd_ = -1;           // Channel拥有的fd，Channel和fd一一对应
    // Epoll* ep_ = nullptr;   // Channel对应的红黑树，Channel与epoll是多对一的关系，一个Channel只对应一个epoll
    Eventloop* loop_;
    bool inepoll_ = false;  // Channel是否已添加到epoll树上，如果没有，则调用epoll_ctl()的时候使用EPOLL_CTL_ADD，否则调用EPOLL_CTL_MOD
    uint32_t events_ = 0;   // fd_需要监视的事件, listenfd和clientfd需要监视EPOLLIN，clientfd可能还需要监视EPOLLOUT
    uint32_t revents_ = 0;  // fd_已发生的事件

    std::function<void()> readcallback_;    // fd_读事件的回调函数

    std::function<void()> closecallback_;
    std::function<void()> errorcallback_;

public:
    Channel(Eventloop* loop, int fd);
    ~Channel();
    int fd();
    void useet();   // 采用边缘触发
    void enablereading();   // 让epoll_wait()监视fd_的读事件
    void setinepoll();  // 把inepoll_成员的值设置为true
    void setrevents(uint32_t ev);   // 设置revents_成员的参数为ev
    bool inpoll();  // 返回inepoll_成员
    uint32_t events();
    uint32_t revents();

    void handleevent(); // 事件处理函数，epoll_wait()返回的时候，执行

    void onmessage();   // 处理对端发来的报文
    void setreadcallback(std::function<void()> fn); // 设置fd_读事件的回调函数

    void setclosecallback(std::function<void()> fn);
    void seterrorcallback(std::function<void()> fn);
};
