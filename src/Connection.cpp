#include "../include/Connection.h"

Connection::Connection(Eventloop *loop, Socket *clientsock)
    :loop_(loop), clientsock_(clientsock)
{
    clientChannel_ = new Channel(loop_, clientsock_->fd());
    clientChannel_->setreadcallback(std::bind(&Channel::onmessage, clientChannel_));  // 设置绑定
    clientChannel_->useet();
    clientChannel_->enablereading();
}

Connection::~Connection()
{
    delete clientChannel_;
    delete clientsock_;
}
