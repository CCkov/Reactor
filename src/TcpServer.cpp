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
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newConnection(Socket* clientsock)
{
    Connection* conn = new Connection(&loop_, clientsock);
}
