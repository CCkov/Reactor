#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Eventloop.h"

class Connection
{
private:
    Socket* clientsock_;
    Eventloop* loop_;   // Acceptor对应的事件循环，在构造函数中传入，因此不能在析构函数中释放
    Channel* clientChannel_;

    std::function<void(Connection*)> closecallback_;
    std::function<void(Connection*)> errorcallback_; 
public:
    Connection(Eventloop* loop, Socket* clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void closecallback();
    void errorcallback();

    void setclosecallback(std::function<void(Connection*)> fn);
    void seterrorcallback(std::function<void(Connection*)> fn);
};

