#pragma once
#include "Eventloop.h"
#include "Socket.h"
#include "Channel.h"

class TcpServer
{
private:
    Eventloop loop_; //
public:
    TcpServer(const uint16_t port);
    ~TcpServer();
    
    void start();
};

