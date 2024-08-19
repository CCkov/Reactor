#include "../include/Acceptor.h"

Acceptor::Acceptor(Eventloop *loop, uint16_t port)
    :loop_(loop)
{
    servsock_ = new Socket(createNoblocking());
    InetAddress serveraddr(port);
    servsock_->setreuseaddr(true);
    servsock_->settcpnodelay(true);
    servsock_->setreuseport(true);
    servsock_->setkeepalive(true);
    servsock_->bind(serveraddr);
    servsock_->listen();

    acceptChannel_ = new Channel(loop_->ep(), servsock_->fd());
    acceptChannel_->setreadcallback(std::bind(&Channel::newconnection, acceptChannel_, servsock_));   // 设置绑定
    acceptChannel_->enablereading();
}
Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptChannel_;
}