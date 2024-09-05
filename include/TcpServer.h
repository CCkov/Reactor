#pragma once
#include "Eventloop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>
#include <functional>
#include <vector>

class TcpServer
{
private:
    Eventloop* mainloop_; // 主事件循环
    std::vector<Eventloop*> subloops_;  // 存放从事件循环的容器
    Acceptor* accrptor_;    // 一个TcpServer只用一个Acceptor对象
    ThreadPool* threadpool_;    // 线程池
    int threadnum_;
    std::map<int, Connection*> conns_;  // 一个TcpServer中有多个Connection对象，存放在map容器中

    std::function<void(Connection*)> newconnectioncallback_;
    std::function<void(Connection*)> closeconnectioncallback_;
    std::function<void(Connection*)> errorconnectioncallback_;
    std::function<void(Connection*, std::string &message)> onmessagecallback_; 
    std::function<void(Connection*)> sendcomplatecallback_;
    std::function<void(Eventloop*)> timeoutcallback_;
public:
    TcpServer(const uint16_t port, int threadnum = 3);
    ~TcpServer();
    
    void start();

    void newConnection(Socket* clientsock);
    void closeconnection(Connection* conn); // 关闭客户端连接，在Connection类中回调此函数
    void errorconnection(Connection* conn); // 客户端的连接错误，在Connection类中回调此函数
    void onmessage(Connection* conn, std::string& message);    // 处理客户端的请求报文，在Connection类中回调此函数
    void sendcomplate(Connection* conn);    // 数据发送完成后，在Connection类中调用此函数
    void epolltimeout(Eventloop* loop);     // epoll_wait()超时，在eventloop类中回调

    void setnewconnectioncallback(std::function<void(Connection*)> fn);
    void setcloseconnectioncallback(std::function<void(Connection*)> fn);
    void seterrorconnectioncallback(std::function<void(Connection*)> fn);
    void setonmessagecallback(std::function<void(Connection*, std::string &message)> fn);
    void setsendcomplatecallback(std::function<void(Connection*)> fn);
    void settimeoutcallback(std::function<void(Eventloop*)> fn);
};

