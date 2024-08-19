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
        std::vector<Channel*> channels = ep_->loop();
        for (auto &ch : channels)
        {
            ch->handleevent();
        } 
    } 
}

Epoll *Eventloop::ep()
{
    return ep_;
}
