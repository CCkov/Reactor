#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Eventloop.h"
#include "Buffer.h"

class Connection
{
private:
    Socket* clientsock_;
    Eventloop* loop_;   // Acceptor对应的事件循环，在构造函数中传入，因此不能在析构函数中释放
    Channel* clientChannel_;

    Buffer inputbuffer_;
    Buffer outputbuffer_;

    std::function<void(Connection*)> closecallback_;
    std::function<void(Connection*)> errorcallback_; 
    std::function<void(Connection*, std::string&)> onmessagecallback_; 
    std::function<void(Connection*)> sendcomplatecallback_; // 处理报文的回调函数，将回调TcpServer::onmessage()
    
public:
    Connection(Eventloop* loop, Socket* clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void closecallback();
    void errorcallback();
    void onmessage();   // 处理对端发来的报文
    void writecallback();

    void setclosecallback(std::function<void(Connection*)> fn);
    void seterrorcallback(std::function<void(Connection*)> fn);
    void setonmessagecallback(std::function<void(Connection*, std::string&)> fn);
    void setsendcomplatecallback(std::function<void(Connection*)> fn);

    void send(const char* data, size_t size);
};

