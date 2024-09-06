#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include "Channel.h"

class Channel;

class Epoll
{
private:
    int epollfd_ = -1;  
    static const int Maxsize = 100;
    epoll_event events_[Maxsize];
public:
    Epoll(/* args */);
    ~Epoll();

    // void addfd(int fd, uint32_t op);
    void updatechannel(Channel* ch);    // 把Channel添加/更新到红黑树上，Channel中有fd，也有需要监视的事件

    // std::vector<epoll_event> loop(int timeout = -1);
    std::vector<Channel*> loop(int timeout = -1);

    void removechannel(Channel* ch);
};


