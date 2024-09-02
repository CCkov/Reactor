#include "../include/Connection.h"

Connection::Connection(Eventloop *loop, Socket *clientsock)
    :loop_(loop), clientsock_(clientsock)
{
    clientChannel_ = new Channel(loop_, clientsock_->fd());

    clientChannel_->setreadcallback(std::bind(&Connection::onmessage, this));  // 设置绑定
    clientChannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientChannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientChannel_->setwritecallback(std::bind(&Connection::writecallback, this));

    clientChannel_->useet();
    clientChannel_->enablereading();
}

Connection::~Connection()
{
    delete clientChannel_;
    delete clientsock_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    closecallback_(this);
}

void Connection::errorcallback()
{
    errorcallback_(this);
}

void Connection::onmessage()
{
    char buff[1024];
    // string buff;
    while (true)
    {
        bzero(&buff, sizeof(buff));
        ssize_t nread = read(fd(), buff, sizeof(buff));

        if (nread > 0)
        {
            // printf("recv(eventfd = %d)::%s\n", fd(), buff);
            // send(fd(), buff, strlen(buff), 0);
            inputbuffer_.append(buff, nread);
        }
        else if ((nread == -1) && (errno == EINTR))
        {
            continue;
        }
        else if ((nread == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            while (true)
            {
                int len;
                memcpy(&len, inputbuffer_.data(), 4);
                if (inputbuffer_.size() < len+4) break;
                std::string message(inputbuffer_.data()+4, len);
                inputbuffer_.erase(0, len+4);

                printf("message(eventfd = %d)::%s\n", fd(), message.data());
                onmessagecallback_(this, message);
                
            }
            break;
        }
        else if (nread == 0)
        {
            
            closecallback();
            break;
        }   
    }
}

void Connection::writecallback()
{
    int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if (writen > 0) outputbuffer_.erase(0, writen);
    
    // 如果发送缓冲区没有数据了, 表示数据已发送成功，不在关注写事件
    if (outputbuffer_.size() == 0) clientChannel_->diablewriting();

}

void Connection::seterrorcallback(std::function<void(Connection *)> fn)
{
    closecallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(Connection *, std::string)> fn)
{
    onmessagecallback_ = fn;
}

void Connection::send(const char *data, size_t size)
{
    outputbuffer_.append(data, size);
    clientChannel_->enablewriting();// 注册写事件

}

void Connection::setclosecallback(std::function<void(Connection *)> fn)
{
    errorcallback_ = fn;
}
