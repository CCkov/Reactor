#include "./include/TcpServer.h"
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
    
    TcpServer tcpserver(atoi(argv[1]));

    tcpserver.start();
    

    return 0;
}

