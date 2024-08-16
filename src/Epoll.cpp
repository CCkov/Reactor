#include "../include/Epoll.h"

Epoll::Epoll()
{
    if ((epollfd_ =epoll_create(1)) == -1) 
    {
        printf("epoll_create1() failed(%d)\n", errno);
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
}

// void Epoll::addfd(int fd, uint32_t op)
// {
//     epoll_event ev;
//     ev.data.fd = fd;
//     ev.events = op;

//     if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
//     {
//         printf("epoll_ctl() failed(%d)\n", errno);
//         exit(-1);
//     }
    
// }

void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();

    if (ch->inpoll())
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            printf("epoll_ctl() failed(%d)\n", errno);
            exit(-1);
        }
        
    }else{
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            printf("epoll_ctl() failed(%d)\n", errno);
            exit(-1);
        }
        ch->setinepoll();   // 把Channel的inepoll_成员设置为true
    }
    
}

// std::vector<epoll_event> Epoll::loop(int timeout)
// {
//     std::vector<epoll_event> evs;
//     bzero(events_, sizeof(events_));

//     int infds = epoll_wait(epollfd_, events_, Maxsize, timeout);
//     if (infds < 0)
//     {
//         perror("epoll_wait() failed\n");
//         exit(-1);
//     }
//     if (infds == 0)
//     {
//         printf("epoll_wait() timeout\n");
//         return evs;
//     }

//     for (int i = 0; i < infds; i++)
//     {
//         evs.push_back(events_[i]);
//     }

//     return evs;
// }

std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel*> channels;
    bzero(events_, sizeof(events_));
    
    int infds = epoll_wait(epollfd_, events_, Maxsize, timeout);
    if (infds < 0)
    {
        perror("epoll_wait() failed\n");
        exit(-1);
    }
    if (infds == 0)
    {
        printf("epoll_wait() timeout\n");
        return channels;
    }

    for (int i = 0; i < infds; i++)
    {
        Channel* ch = (Channel*)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        channels.push_back(ch);
    }

    return channels;
}



/*
    1、EPOLLRDHUP：事件通常用于检测对端关闭连接或关闭写操作。

    2、EPOLLIN：表示有普通数据可以读取（如套接字收到新数据包）。
        EPOLLPRI：表示有紧急数据可以读取（如TCP带外数据）

    3、EPOLLOUT：表示文件描述符可写，意味着可以往套接字中写入数据。
*/