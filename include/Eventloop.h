#pragma once
#include "../include/Epoll.h"

class Eventloop
{
private:
    Epoll* ep_; // 每个事件循环只有一个epoll
public:
    Eventloop();    // 在构造函数中创建epoll对象ep_
    ~Eventloop();   // 在析构函数中销毁ep_

    void run(); // 运行事件循环
    Epoll* ep();
};


