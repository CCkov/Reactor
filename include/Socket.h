#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

int createNoblocking();

class Socket
{
private:
    const int fd_;
    std::string ip_;    // 如果是客户端连接的fd，存放对端的ip
    uint16_t port_;     // 如果是listenfd，存放服务端监听的port， 如果是客户端连接的fd，存放对端的port
public:
    Socket(int fd);
    ~Socket();

    int fd() const;

    std::string ip() const;
    uint16_t port() const;

    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);
    void setkeepalive(bool on);

    void bind(const InetAddress& serveraddr);
    void listen(int n = 128);

    int accept(InetAddress& clientaddr);

};

