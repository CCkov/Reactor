#include "../include/Connection.h"

Connection::Connection(Eventloop *loop, Socket *clientsock)
    :loop_(loop), clientsock_(clientsock)
{
    clientChannel_ = new Channel(loop_, clientsock_->fd());

    clientChannel_->setreadcallback(std::bind(&Channel::onmessage, clientChannel_));  // 设置绑定
    clientChannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientChannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));

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
    printf("1客户端(eventfd=%d) 断开连接.\n", fd());
    close(fd());
}

void Connection::errorcallback()
{
    printf("3客户端(eventfd=%d) 发生错误.\n", fd());
    close(fd());
}
