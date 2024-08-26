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
    closecallback_(this);
}

void Connection::errorcallback()
{
    errorcallback_(this);
}

void Connection::seterrorcallback(std::function<void(Connection *)> fn)
{
    closecallback_ = fn;
}

void Connection::setclosecallback(std::function<void(Connection *)> fn)
{
    errorcallback_ = fn;
}
