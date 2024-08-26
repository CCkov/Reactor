#include "../include/TcpServer.h"
#include "../include/Connection.h"

TcpServer::TcpServer(const uint16_t port)
{
    accrptor_ = new Acceptor(&loop_, port);
    accrptor_->setnewConnectioncb(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete accrptor_;
    
    // 释放全部Connection对象
    for (auto &i : conns_)
    {
        delete i.second;
    }
    
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newConnection(Socket* clientsock)
{
    Connection* conn = new Connection(&loop_, clientsock);

    printf("接受客户端连接(fd=%d,ip=%s,port=%d) 成功.\n", conn->fd(), conn->ip().c_str(), conn->port());

    conns_[conn->fd()] = conn;  // 把conn存放到map容器中

}
