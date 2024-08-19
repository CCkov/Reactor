#include "../include/TcpServer.h"

TcpServer::TcpServer(const uint16_t port)
{
    Socket* servsock = new Socket(createNoblocking());
    InetAddress serveraddr(port);
    servsock->setreuseaddr(true);
    servsock->settcpnodelay(true);
    servsock->setreuseport(true);
    servsock->setkeepalive(true);
    servsock->bind(serveraddr);
    servsock->listen();

    Channel* servChannel = new Channel(loop_.ep(), servsock->fd());
    servChannel->setreadcallback(std::bind(&Channel::newconnection, servChannel, servsock));   // 设置绑定
    servChannel->enablereading();
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    loop_.run();
}
