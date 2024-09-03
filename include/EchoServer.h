#pragma once
#include "TcpServer.h"
#include "Eventloop.h"
#include "Connection.h"

class TcpServer;

class EchoServer
{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const uint16_t port);
    ~EchoServer();

    void start();   // 启动服务

    void HandleNewConnection(Connection* conn);

    void HandleClose(Connection* conn); // 关闭客户端连接，在Connection类中回调此函数
    void HandleError(Connection* conn); // 客户端的连接错误，在Connection类中回调此函数
    void HandleMessage(Connection* conn, std::string& message);    // 处理客户端的请求报文，在Connection类中回调此函数
    void HandleSendComplate(Connection* conn);    // 数据发送完成后，在Connection类中调用此函数
    void HandleTimeout(Eventloop* loop);     // epoll_wait()超时，在eventloop类中回调
};