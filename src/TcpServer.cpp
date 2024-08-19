#include "../include/TcpServer.h"

TcpServer::TcpServer(const uint16_t port)
{
    accrptor_ = new Acceptor(&loop_, port);
}

TcpServer::~TcpServer()
{
    delete accrptor_;
}

void TcpServer::start()
{
    loop_.run();
}
