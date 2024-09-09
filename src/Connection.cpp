#include "../include/Connection.h"

Connection::Connection(Eventloop* loop, std::unique_ptr<Socket> clientsock)
    :loop_(loop), clientsock_(std::move(clientsock)), disconnect_(false), clientChannel_(new Channel(loop_, clientsock_->fd()))
{
    // clientChannel_ = new Channel(loop_, clientsock_->fd());
    
    clientChannel_->setreadcallback(std::bind(&Connection::onmessage, this));  // 设置绑定
    clientChannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientChannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientChannel_->setwritecallback(std::bind(&Connection::writecallback, this));

    clientChannel_->useet();
    clientChannel_->enablereading();
}

Connection::~Connection()
{
    printf("conn析构\n");

}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    disconnect_ = true;
    closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
    errorcallback_(shared_from_this());
}

void Connection::onmessage()
{
    char buff[1024];
    // string buff;
    while (true)
    {
        bzero(&buff, sizeof(buff));
        ssize_t nread = read(fd(), buff, sizeof(buff));

        if (nread > 0)
        {
            // printf("recv(eventfd = %d)::%s\n", fd(), buff);
            // send(fd(), buff, strlen(buff), 0);
            inputbuffer_.append(buff, nread);
        }
        else if ((nread == -1) && (errno == EINTR))
        {
            continue;
        }
        else if ((nread == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            while (true)
            {
                int len;
                memcpy(&len, inputbuffer_.data(), 4);
                if (inputbuffer_.size() < len+4) break;
                std::string message(inputbuffer_.data()+4, len);
                inputbuffer_.erase(0, len+4);

                printf("message(eventfd = %d)::%s\n", fd(), message.data());
                lasttime_ = Timestamp::now();
                onmessagecallback_(shared_from_this(), message);
                
            }
            break;
        }
        else if (nread == 0)
        {
            clientChannel_->remove();
            closecallback();
            break;
        }   
    }
}

void Connection::writecallback()
{
    int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if (writen > 0) outputbuffer_.erase(0, writen);
    
    // 如果发送缓冲区没有数据了, 表示数据已发送成功，不在关注写事件
    if (outputbuffer_.size() == 0)
    {
        clientChannel_->diablewriting();
        sendcomplatecallback_(shared_from_this());
    }

}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)
{
    closecallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(spConnection, std::string&)> fn)
{
    onmessagecallback_ = fn;
}

void Connection::setsendcomplatecallback(std::function<void(spConnection)> fn)
{
    sendcomplatecallback_ = fn;
}

void Connection::send(const char *data, size_t size)
{
    if (disconnect_ == true)
    {
        printf("客户端已断开,send()直接返回\n");
        return;
    }

    if (loop_->isinloopthread())
    {
        // 如果当先线程是IO线程，直接执行发送数据的操作
        printf("在IO线程中\n");
        sendinloop(data, size);
    }else
    {
        // 如果当先线程不是IO线程，调用Eventloop::queueinloop(), 把发送数据的操作交给事件循环线程去执行
        printf("不在IO线程中\n");
        loop_->queueinloop(std::bind(&Connection::sendinloop, this, data, size));
    }
}

void Connection::sendinloop(const char *data, size_t size)
{
    outputbuffer_.appendwithhead(data, size);
    clientChannel_->enablewriting();// 注册写事件
}

bool Connection::timeout(time_t now, int val)
{
    return now - lasttime_.toint() > val;
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)
{
    errorcallback_ = fn;
}
