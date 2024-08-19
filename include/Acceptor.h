#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Eventloop.h"

class Acceptor
{
private:
    Socket* servsock_;
    Eventloop* loop_;   // Acceptor对应的事件循环，在构造函数中传入，因此不能在析构函数中释放
    Channel* acceptChannel_;
public:
    Acceptor(Eventloop* loop, uint16_t port);
    ~Acceptor();

    void newConnection();
};

