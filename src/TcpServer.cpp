#include "../include/TcpServer.h"
#include "../include/Connection.h"

TcpServer::TcpServer(const uint16_t port, int threadnum)
    :threadnum_(threadnum)
{
    mainloop_ = new Eventloop;
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));

    accrptor_ = new Acceptor(mainloop_, port);
    accrptor_->setnewConnectioncb(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

    // 创建从事件循环
    threadpool_ = new ThreadPool(threadnum_);
    for (int i = 0; i < threadnum_; i++)
    {
        subloops_.push_back(new Eventloop); // 创建事件循环，存入subloops_容器中
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        threadpool_->addtask(std::bind(&Eventloop::run, subloops_[i])); // 在线程池中运行事件循环
    }
    
}

TcpServer::~TcpServer()
{
    delete accrptor_;
    delete mainloop_;
    
    // 释放全部Connection对象
    for (auto &i : conns_)
    {
        delete i.second;
    }
    
    for (auto &i : subloops_)
    {
        delete i;
    }
    delete threadpool_;
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::newConnection(Socket* clientsock)
{
    // Connection* conn = new Connection(mainloop_, clientsock);
    Connection* conn = new Connection(subloops_[clientsock->fd() % threadnum_], clientsock);

    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcomplatecallback(std::bind(&TcpServer::sendcomplate, this, std::placeholders::_1));
    
    printf("接受客户端连接(fd=%d,ip=%s,port=%d) 成功.\n", conn->fd(), conn->ip().c_str(), conn->port());

    conns_[conn->fd()] = conn;  // 把conn存放到map容器中

    newconnectioncallback_(conn);   // 回调EchoServer::HandleNewConnection()

}

void TcpServer::closeconnection(Connection *conn)
{
    closeconnectioncallback_(conn); // 回调EchoServer::
    printf("1客户端(eventfd=%d) 断开连接.\n", conn->fd());
    // close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection *conn)
{
    errorconnectioncallback_(conn);
    printf("3客户端(eventfd=%d) 发生错误.\n", conn->fd());
    // close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection *conn, std::string& message)
{
    onmessagecallback_(conn, message);
}

void TcpServer::sendcomplate(Connection *conn)
{
    // printf("Send complate\n");
    sendcomplatecallback_(conn);
}

void TcpServer::epolltimeout(Eventloop *loop)
{
    // printf("epoll_wait() timeout\n");
    timeoutcallback_(loop);
}

void TcpServer::setnewconnectioncallback(std::function<void(Connection *)> fn)
{
    newconnectioncallback_ = fn;
}

void TcpServer::settimeoutcallback(std::function<void(Eventloop*)> fn)
{
    timeoutcallback_ = fn;
}

void TcpServer::setsendcomplatecallback(std::function<void(Connection *)> fn)
{
    sendcomplatecallback_ = fn;
}

void TcpServer::setonmessagecallback(std::function<void(Connection *, std::string &message)> fn)
{
    onmessagecallback_ = fn;
}

void TcpServer::seterrorconnectioncallback(std::function<void(Connection *)> fn)
{
    errorconnectioncallback_ = fn;
}

void TcpServer::setcloseconnectioncallback(std::function<void(Connection *)> fn)
{
    closeconnectioncallback_ = fn;
}
