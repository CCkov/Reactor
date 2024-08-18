#include <iostream>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include "InetAddress.h"

using namespace std;

/*
封装InetAddress类
*/

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("./server 端口");
        return -1;
    }

    int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("listenfd socket() failed\n");
        return -1;
    }

    int opt = 1;

    /* 
    1、SO_REUSEADDR:这个选项允许在本地地址已被使用的情况下重用这个地址。
        开启这个选项后，可以让服务器程序在重启时更快地重新绑定端口，减少资源等待时间。

    2、TCP_NODELAY:这个选项禁用了Nagle算法，
        Nagle算法用于减少网络中小数据包的数量，通过将小数据包合并发送。
        但是在某些情况下（比如需要实时响应的应用），可能会需要关闭Nagle算法来避免延迟，这时就可以设置TCP_NODELAY。

    3、SO_REUSEPORT:这个选项允许多个套接字绑定到同一个IP地址和端口。
        它通常用于多线程服务器程序中，可以让多个进程或线程在同一端口上监听并接收连接请求，以实现更好的负载均衡。
    
    4、SO_KEEPALIVE:这个选项用于开启TCP的“保活”功能。
        如果开启此选项，TCP会定期发送保活探测包给对端，检测对方是否仍然存活。
        此功能可以帮助在长时间没有数据传输的情况下检测到连接是否仍然有效，避免因为对端崩溃或网络故障导致的长时间无响应。
    */
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
    
    InetAddress serveraddr(atoi(argv[1]));

    if (bind(listenfd, serveraddr.addr(), sizeof(sockaddr)) < 0)
    {
        perror("bind() failed\n");
        close(listenfd);
        return -1;
    }
    if (listen(listenfd, 128) != 0)
    {
        perror("listen() failed\n");
        close(listenfd);
        return -1;
    }

    int epollfd =epoll_create(1);
    if (epollfd == -1) {
    perror("epoll_create1() failed");
    close(listenfd);
    return -1;
    }
    epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
    
    struct epoll_event evs[10];

    while (true)
    {
        int infds = epoll_wait(epollfd, evs, 10, -1);
        if (infds < 0)
        {
            perror("epoll_wait() failed\n");
            break;
        }
        if (infds == 0)
        {
            cout << "epoll_wait() timeout\n";
            continue;
        }

        /*
        1、EPOLLRDHUP：事件通常用于检测对端关闭连接或关闭写操作。

        2、EPOLLIN：表示有普通数据可以读取（如套接字收到新数据包）。
           EPOLLPRI：表示有紧急数据可以读取（如TCP带外数据）

        3、EPOLLOUT：表示文件描述符可写，意味着可以往套接字中写入数据。
        */

        for (int i = 0; i < infds; i++)
        {
            
            if (evs[i].events & EPOLLRDHUP)
            {
                printf("1客户端(eventfd=%d) 断开连接.\n", evs[i].data.fd);
                close(evs[i].data.fd);
            }
            else if (evs[i].events & EPOLLIN|EPOLLPRI)
            {
                if (evs[i].data.fd == listenfd)
                {
                    struct sockaddr_in peeradrdr;
                    socklen_t len = sizeof(peeradrdr);
                    int clientfd = accept4(listenfd, (struct sockaddr*)&peeradrdr, &len, SOCK_NONBLOCK);
                    
                    InetAddress clientaddr(peeradrdr);
                    
                    if (clientfd < 0)
                    {
                        perror("accept4() failed\n");
                        return -1;
                    }
                
                    printf("接受客户端连接(fd=%d,ip=%s,port=%d) 成功.\n", clientfd, clientaddr.ip(), clientaddr.port());

                    ev.data.fd = clientfd;
                    ev.events = EPOLLIN|EPOLLET;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                }
                else
                {
                    char buff[1024];
                    // string buff;
                    while (true)
                    {
                        bzero(&buff, sizeof(buff));
                        ssize_t nread = read(evs[i].data.fd, buff, sizeof(buff));

                        if (nread > 0)
                        {
                            printf("recv(eventfd = %d)::%s\n", evs[i].data.fd, buff);
                            send(evs[i].data.fd, buff, strlen(buff), 0);
                        }
                        else if ((nread == -1) && (errno == EINTR))
                        {
                            continue;
                        }
                        else if ((nread == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                        {
                            break;
                        }
                        else if (nread == 0)
                        {
                            printf("2客户端(eventfd=%d) 断开连接.\n", evs[i].data.fd);
                            close(evs[i].data.fd);
                            break;
                        }   
                    }
                    
                }
            }
            else if (evs[i].events & EPOLLOUT)
            {
                /* code */
            }
            else{
                printf("3客户端(eventfd=%d) 发生错误.\n", evs[i].data.fd);
                close(evs[i].data.fd);
            }
            } 
        }  
    

    return 0;
}

