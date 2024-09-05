#include "../include/EchoServer.h"

EchoServer::EchoServer(const uint16_t port, int threadnum)
    :tcpserver_(port, threadnum)
{
    tcpserver_.setnewconnectioncallback(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncallback(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncallback(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecallback(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcomplatecallback(std::bind(&EchoServer::HandleSendComplate, this, std::placeholders::_1));
    tcpserver_.settimeoutcallback(std::bind(&EchoServer::HandleTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}

void EchoServer::start()
{
    tcpserver_.start();
}

void EchoServer::HandleNewConnection(Connection *conn)
{
    std::cout << "New Connection Come in." << std::endl;
    
    printf("EchoServer::HandleNewConnection() thread is %ld\n", syscall(SYS_gettid));
}

void EchoServer::HandleClose(Connection *conn)
{
    std::cout << "EehoServer conn closed." << std::endl;
}

void EchoServer::HandleSendComplate(Connection *conn)
{
    std::cout << "Message send complate." << std::endl;
}

void EchoServer::HandleTimeout(Eventloop *loop)
{
    std::cout << "EchoServer timeout." << std::endl;
}

void EchoServer::HandleMessage(Connection *conn, std::string& message)
{
    printf("EchoServer::HandleMessage() thread is %ld\n", syscall(SYS_gettid));
    
    message = "reply:" + message;
    
    // send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);

    conn->send(message.data(), message.size()); // 把临时缓冲区中的数据直接send 出去
}

void EchoServer::HandleError(Connection *conn)
{
    std::cout << "EehoServer conn error." << std::endl;
}
