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

    void closeconnection(Connection* conn); // 关闭客户端连接，在Connection类中回调此函数
    void errorconnection(Connection* conn); // 客户端的连接错误，在Connection类中回调此函数

    void onmessage(Connection* conn, std::string message);    // 处理客户端的请求报文，在Connection类中回调此函数
};

