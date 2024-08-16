#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
private:
    sockaddr_in addr_;

public:
    InetAddress();
    InetAddress(uint16_t port);  // 监听的listenfd
    InetAddress(const sockaddr_in addr);  // 客户端的clientfd
    ~InetAddress();

    const char* ip() const;
    uint16_t port() const;
    const sockaddr* addr() const;
    void setaddr(sockaddr_in clientaddr);
};


