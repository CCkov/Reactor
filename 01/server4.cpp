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
#include "./include/InetAddress.h"
#include "./include/Socket.h"
#include "./include/Epoll.h"

using namespace std;

/*
封装Socket类
*/

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("./server 端口");
        return -1;
    }

    Socket servsock(createNoblocking());
    InetAddress serveraddr(atoi(argv[1]));
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);
    servsock.bind(serveraddr);
    servsock.listen();

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
    
    Epoll ep;
    ep.addfd(servsock.fd(), EPOLLIN);
    std::vector<epoll_event> evs;

    while (true)
    {
        evs = ep.loop();
        

        // for (int i = 0; i < infds; i++)
        for (auto &ev : evs)
        {
            
            if (ev.events & EPOLLRDHUP)
            {
                printf("1客户端(eventfd=%d) 断开连接.\n", ev.data.fd);
                close(ev.data.fd);
            }
            else if (ev.events & EPOLLIN|EPOLLPRI)
            {
                if (ev.data.fd == servsock.fd())
                {
                    InetAddress clientaddr;
                    Socket* clientsock = new Socket(servsock.accept(clientaddr));
                 
                    printf("接受客户端连接(fd=%d,ip=%s,port=%d) 成功.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    ep.addfd(clientsock->fd(), EPOLLIN|EPOLLET);
                }
                else
                {
                    char buff[1024];
                    // string buff;
                    while (true)
                    {
                        bzero(&buff, sizeof(buff));
                        ssize_t nread = read(ev.data.fd, buff, sizeof(buff));

                        if (nread > 0)
                        {
                            printf("recv(eventfd = %d)::%s\n", ev.data.fd, buff);
                            send(ev.data.fd, buff, strlen(buff), 0);
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
                            printf("2客户端(eventfd=%d) 断开连接.\n", ev.data.fd);
                            close(ev.data.fd);
                            break;
                        }   
                    }
                    
                }
            }
            else if (ev.events & EPOLLOUT)
            {
                /* code */
            }
            else{
                printf("3客户端(eventfd=%d) 发生错误.\n", ev.data.fd);
                close(ev.data.fd);
            }
            } 
        }  
    

    return 0;
}

