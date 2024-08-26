#pragma once
#include "Eventloop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>

class TcpServer
{
private:
    Eventloop loop_; //
    Acceptor* accrptor_;
    std::map<int, Connection*> conns_;  // 一个TcpServer中有多个Connection对象，存放在map容器中
public:
    TcpServer(const uint16_t port);
    ~TcpServer();
    
    void start();

    void newConnection(Socket* clientsock);
};

