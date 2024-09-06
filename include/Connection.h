#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Eventloop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>

class Connection;
using spConnection = std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    Socket* clientsock_;
    Eventloop* loop_;   // Connection对应的事件循环，在构造函数中传入
    Channel* clientChannel_;    // 与Connection对应的Channel，在构造函数中创建

    Buffer inputbuffer_;
    Buffer outputbuffer_;

    std::atomic_bool disconnect_;   // 客户端连接是否已断开，如果断开，则设置为true

    std::function<void(spConnection)> closecallback_;
    std::function<void(spConnection)> errorcallback_; 
    std::function<void(spConnection, std::string&)> onmessagecallback_; 
    std::function<void(spConnection)> sendcomplatecallback_; // 处理报文的回调函数，将回调TcpServer::onmessage()
    
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

    void setclosecallback(std::function<void(spConnection)> fn);
    void seterrorcallback(std::function<void(spConnection)> fn);
    void setonmessagecallback(std::function<void(spConnection, std::string&)> fn);
    void setsendcomplatecallback(std::function<void(spConnection)> fn);

    void send(const char* data, size_t size);
};

