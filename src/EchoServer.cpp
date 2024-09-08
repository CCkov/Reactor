#include "../include/EchoServer.h"

EchoServer::EchoServer(const uint16_t port, int subthreadnum, int workthreadnum)
    :tcpserver_(port, subthreadnum),threadpool_(workthreadnum, "WORKS")
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

void EchoServer::HandleNewConnection(spConnection conn)
{
    std::cout << "New Connection Come in." << std::endl;
    
    // printf("EchoServer::HandleNewConnection() thread is %ld\n", syscall(SYS_gettid));
}

void EchoServer::HandleClose(spConnection conn)
{
    std::cout << "EehoServer conn closed." << std::endl;
}

void EchoServer::HandleSendComplate(spConnection conn)
{
    std::cout << "Message send complate." << std::endl;
}

void EchoServer::HandleTimeout(Eventloop *loop)
{
    std::cout << "EchoServer timeout." << std::endl;
}

// 处理客户端的请求报文，在TcpServer类中回调此函数
void EchoServer::HandleMessage(spConnection conn, std::string &message)
{
    // printf("EchoServer::HandleMessage() thread is %ld\n", syscall(SYS_gettid));

    if (threadpool_.size() == 0)
    {
        // 如果没有工作线程，表示在IO线程中计算
        OnMessage(conn, message);
    }else
    {
        // 把任务添加到线程池的任务队列中
        threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
    }
    
}

void EchoServer::OnMessage(spConnection conn, std::string &message)
{
    message = "reply:" + message;
    // send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn->send(message.data(), message.size()); // 把临时缓冲区中的数据直接send 出去
}

void EchoServer::HandleError(spConnection conn)
{
    std::cout << "EehoServer conn error." << std::endl;
}
