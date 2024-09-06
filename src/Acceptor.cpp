#include "../include/Acceptor.h"
#include "../include/Connection.h"

Acceptor::Acceptor(const std::unique_ptr<Eventloop>& loop, uint16_t port)
    :loop_(loop)
{
    servsock_ = new Socket(createNoblocking());
    InetAddress serveraddr(port);
    servsock_->setreuseaddr(true);
    servsock_->settcpnodelay(true);
    servsock_->setreuseport(true);
    servsock_->setkeepalive(true);
    servsock_->bind(serveraddr);
    servsock_->listen();

    acceptChannel_ = new Channel(loop_, servsock_->fd());
    acceptChannel_->setreadcallback(std::bind(&Acceptor::newConnection, this));   // 设置绑定
    acceptChannel_->enablereading();
}
Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptChannel_;
}

void Acceptor::newConnection()
{
    InetAddress clientaddr;
    std::unique_ptr<Socket> clientsock(new Socket(servsock_->accept(clientaddr)));
                 
    // printf("接受客户端连接(fd=%d,ip=%s,port=%d) 成功.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());
    clientsock->setipport(clientaddr.ip(), clientaddr.port());
    newConnectioncd_(std::move(clientsock));    // 回调TcpServer::newConnection()
}

void Acceptor::setnewConnectioncb(std::function<void(std::unique_ptr<Socket>)> fn)
{
    newConnectioncd_ = fn;
}