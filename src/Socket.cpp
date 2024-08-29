#include "../include/Socket.h"

int createNoblocking()
{
    int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        // perror("listenfd socket() failed\n");
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd):fd_(fd)
{
}

int Socket::fd() const
{
    return fd_;
}

std::string Socket::ip() const
{
    return ip_;
}

uint16_t Socket::port() const
{
return port_;
}

void Socket::setipport(const std::string& ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;
}

void Socket::setreuseaddr(bool on)
{
    int optval = on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setreuseport(bool on)
{
    int optval = on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::settcpnodelay(bool on)
{
    int optval = on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setkeepalive(bool on)
{
    int optval = on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::bind(const InetAddress &serveraddr)
{
    if (::bind(fd_, serveraddr.addr(), sizeof(sockaddr)) < 0)
    {
        perror("bind() failed\n");
        close(fd_);
        exit(-1);
    }
    setipport(serveraddr.ip(), serveraddr.port());
}

int Socket::accept(InetAddress &clientaddr)
{
    struct sockaddr_in peeradrdr;
    socklen_t len = sizeof(peeradrdr);
    int clientfd = accept4(fd_, (struct sockaddr*)&peeradrdr, &len, SOCK_NONBLOCK);
                    
    clientaddr.setaddr(peeradrdr);
                    
    if (clientfd < 0)
    {
        perror("accept4() failed\n");
        exit(-1);
    }

    // ip_ = clientaddr.ip();
    // port_ = clientaddr.port();



    return clientfd;
}

void Socket::listen(int n)
{
    if (::listen(fd_, n) != 0)
    {
        perror("listen() failed\n");
        close(fd_);
        exit(-1);
    }
}

Socket::~Socket()
{
    ::close(fd_);
}


