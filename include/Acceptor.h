#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Eventloop.h"
#include <memory>

class Acceptor
{
private:
    Socket* servsock_;  // 构造函数中创建

    // Eventloop* loop_;   // Acceptor对应的事件循环, 在构造函数中传入, 因此不能在析构函数中释放
    const std::unique_ptr<Eventloop>& loop_;

    Channel* acceptChannel_;    // 构造函数中创建

    std::function<void(std::unique_ptr<Socket>)>  newConnectioncd_;
public:
    Acceptor(const std::unique_ptr<Eventloop>& loop, uint16_t port);
    ~Acceptor();

    void newConnection();

    void setnewConnectioncb(std::function<void(std::unique_ptr<Socket>)>);
};

