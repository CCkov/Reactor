#include "../include/Eventloop.h"

Eventloop::Eventloop():ep_(new Epoll)
{
}

Eventloop::~Eventloop()
{
    delete ep_;
}

void Eventloop::run()
{
    while (true)
    {
        std::vector<Channel*> channels = ep_->loop(10*1000);

        // 如果Channel为空，表示超时，回调TcpServer::epolltimeout()
        if (channels.size() == 0)
        {
            epolltimeoutcallback_(this);
        }else
        {
            for (auto &ch : channels)
            {
                ch->handleevent();
            }
        }
        
         
    } 
}

Epoll *Eventloop::ep()
{
    return ep_;
}

void Eventloop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void Eventloop::setepolltimeoutcallback(std::function<void(Eventloop *)> fn)
{
    epolltimeoutcallback_ = fn;
}
