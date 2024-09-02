#include "../include/Channel.h"
#include "../include/InetAddress.h"
#include "../include/Socket.h"
#include "../include/Eventloop.h"

Channel::Channel(Eventloop* loop, int fd):loop_(loop), fd_(fd)
{

}

Channel::~Channel()
{
}

int Channel::fd()
{
    return fd_;
}

void Channel::useet()
{
    events_ = events_|EPOLLET;
}

// 让epoll_wait()监视fd_的读事件
void Channel::enablereading()
{
    events_ = events_|EPOLLIN;
    // events_ |= EPOLLIN;

    loop_->ep()->updatechannel(this);
}

void Channel::disablereading()
{
    events_ &= ~EPOLLIN;
    loop_->ep()->updatechannel(this);
}

void Channel::enablewriting()
{
    events_ |= EPOLLOUT;
    loop_->ep()->updatechannel(this);
}

void Channel::diablewriting()
{
    events_ &= ~EPOLLOUT;
    loop_->ep()->updatechannel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

bool Channel::inpoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_ ;
}

void Channel::handleevent()
{
    if (revents_ & EPOLLRDHUP)  // 对方已关闭，
    {
        
        closecallback_();
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))   // 接受缓冲区中有数据可以读
    {
        // if (islisten_ == true)
        // {
        //     newconnection(servsock);
        // }
        // else
        // {
        //     onmessage();         
        // }
        readcallback_();
    }
    else if (revents_ & EPOLLOUT)   // 发送缓冲区有数据要发
    {
       printf("revents_ & EPOLLOUT\n");
       writecallback_();
    }
    else{   // 其他事件视为错误

        errorcallback_();
    }
}


void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_ = fn;
}

void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_ = fn;
}
