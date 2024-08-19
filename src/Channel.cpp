#include "../include/Channel.h"
#include "../include/InetAddress.h"
#include "../include/Socket.h"


Channel::Channel(Epoll *ep, int fd):ep_(ep), fd_(fd)
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

    ep_->updatechannel(this);
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
    if (revents_ & EPOLLRDHUP)
    {
        printf("1客户端(eventfd=%d) 断开连接.\n", fd_);
        close(fd_);
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
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
    else if (revents_ & EPOLLOUT)
    {
        /* code */
    }
    else{
        printf("3客户端(eventfd=%d) 发生错误.\n", fd_);
        close(fd_);
    }
}

void Channel::newconnection(Socket *servsock)
{
    InetAddress clientaddr;
    Socket* clientsock = new Socket(servsock->accept(clientaddr));
                 
    printf("接受客户端连接(fd=%d,ip=%s,port=%d) 成功.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

    Channel* clientChannel = new Channel(ep_, clientsock->fd());

    clientChannel->setreadcallback(std::bind(&Channel::onmessage, clientChannel));  // 设置绑定

    clientChannel->useet();
    clientChannel->enablereading();
}

void Channel::onmessage()
{
    char buff[1024];
    // string buff;
    while (true)
    {
        bzero(&buff, sizeof(buff));
        ssize_t nread = read(fd_, buff, sizeof(buff));

        if (nread > 0)
        {
            printf("recv(eventfd = %d)::%s\n", fd_, buff);
            send(fd_, buff, strlen(buff), 0);
        }
        else if ((nread == -1) && (errno == EINTR))
        {
            continue;
        }
        else if ((nread == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            break;
        }
        else if (nread == 0)
        {
            printf("2客户端(eventfd=%d) 断开连接.\n", fd_);
            close(fd_);
            break;
        }   
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}
