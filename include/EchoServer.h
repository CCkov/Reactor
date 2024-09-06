#pragma once
#include "TcpServer.h"
#include "Eventloop.h"
#include "Connection.h"
#include "ThreadPool.h"

class TcpServer;

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_; // 工作线程池
public:
    EchoServer(const uint16_t port, int subthreadnum = 3, int workthreadnum = 5);
    ~EchoServer();

    void start();   // 启动服务

    void HandleNewConnection(spConnection conn);

    void HandleClose(spConnection conn); // 关闭客户端连接，在Connection类中回调此函数
    void HandleError(spConnection conn); // 客户端的连接错误，在Connection类中回调此函数
    void HandleMessage(spConnection conn, std::string& message);    // 处理客户端的请求报文，在Connection类中回调此函数
    void HandleSendComplate(spConnection conn);    // 数据发送完成后，在Connection类中调用此函数
    void HandleTimeout(Eventloop* loop);     // epoll_wait()超时，在eventloop类中回调

    void OnMessage(spConnection conn, std::string& message); // 处理客户端的请求报文，添加到线程池中
};