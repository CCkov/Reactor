#pragma once
#include "Eventloop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"

class TcpServer
{
private:
    Eventloop loop_; //
    Acceptor* accrptor_;
public:
    TcpServer(const uint16_t port);
    ~TcpServer();
    
    void start();
};

