#include "../include/Eventloop.h"

Eventloop::Eventloop()
    :ep_(new Epoll), wakeupfd_(eventfd(0,EFD_NONBLOCK)),wakechannel_(new Channel(this, wakeupfd_))
{
    wakechannel_->setreadcallback(std::bind(&Eventloop::handlewakeup, this));
    wakechannel_->enablereading();
}

Eventloop::~Eventloop()
{
    // delete ep_;
}

void Eventloop::run()
{
    threadid_ = syscall(SYS_gettid);
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

// Epoll *Eventloop::ep()
// {
//     return ep_;
// }

void Eventloop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void Eventloop::setepolltimeoutcallback(std::function<void(Eventloop *)> fn)
{
    epolltimeoutcallback_ = fn;
}

void Eventloop::removechannel(Channel *ch)
{
    ep_->removechannel(ch);
}

bool Eventloop::isinloopthread()
{
    return threadid_ == syscall(SYS_gettid);
}

void Eventloop::queueinloop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }

    wakeup();
}

void Eventloop::wakeup()
{
    uint64_t val = 1;
    write(wakeupfd_, &val, sizeof(val));
}

void Eventloop::handlewakeup()
{
    printf("Eventloop::handlewakeup id is %ld\n", syscall(SYS_gettid));
    uint64_t val;
    read(wakeupfd_, &val, sizeof(val)); // 从eventfd中读取出数据，如果不读取，eventfd的读事件会一直触发

    std::function<void()> fn;

    std::lock_guard<std::mutex> gd(mutex_);

    while (taskqueue_.size() > 0)
    {
        fn = std::move(taskqueue_.front());
        taskqueue_.pop();
        fn();
    }
    
}
