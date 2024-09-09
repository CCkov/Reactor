#include "../include/Eventloop.h"
#include "Eventloop.h"

int Eventloop::createtimerfd(int sec = 30)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(itimerspec));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}


void Eventloop::handletimer()
{
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(itimerspec));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_, 0, &timeout, 0);

    if (mainloop_)
    {
        // printf("主事件闹钟响了\n");

    }else
    {
        // printf("从事件闹钟响了\n");
        time_t now = time(0);
        std::lock_guard<std::mutex> lock_guard(mmutex_);
        for (auto i = conns_.begin(); i != conns_.end();)
        {   
            if (i->second->timeout(now, 10))
            {
                int i_fd = i->first;
                
                i = conns_.erase(i);
                timercallback_(i_fd);
                
            }else{
                ++i;
            }
            
            // if (i.second->timeout(now,10))
            // {
            //     {
            //         std::lock_guard<std::mutex> lock_guard(mutex_);
            //         conns_.erase(i.first);   // 从EventLoop的map中删除超时的conn
            //     }
            //     timercallback_(i.first);   // 从TcpServer的map中删除超时的conn
            // }
            
        }
        
    }
}


/*
void Eventloop::handletimer() {
  struct itimerspec timeout;
  memset(&timeout,0,sizeof(struct itimerspec));
  timeout.it_value.tv_sec = 5;
  timeout.it_value.tv_nsec = 0;
  timerfd_settime(timerfd_,0,&timeout,0);

  if (mainloop_){

  }else{
    time_t now = time(0);
    for(auto conn:conns_){
      if (conn.second->timeout(now,10)){
        {
          std::lock_guard<std::mutex> lock_guard(mutex_);
          conns_.erase(conn.first);   // 从EventLoop的map中删除超时的conn
        }
        timercallback_(conn.first);   // 从TcpServer的map中删除超时的conn
      }
    }
  }
}
*/

Eventloop::Eventloop(bool mainloop, int timetvl, int timeout)
    :timetvl_(timetvl), timeout_(timeout), ep_(new Epoll), wakeupfd_(eventfd(0, EFD_NONBLOCK)), wakechannel_(new Channel(this, wakeupfd_)), timerfd_(createtimerfd()),
      timerchannel_(new Channel(this, timerfd_)), mainloop_(mainloop),stop_(false)
{
    wakechannel_->setreadcallback(std::bind(&Eventloop::handlewakeup, this));
    wakechannel_->enablereading();

    timerchannel_->setreadcallback(std::bind(&Eventloop::handletimer, this));
    timerchannel_->enablereading();
}

Eventloop::~Eventloop()
{
    // delete ep_;
}

void Eventloop::run()
{
    threadid_ = syscall(SYS_gettid);
    while (stop_ == false)
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

void Eventloop::stop()
{
    stop_ = true;
    wakeup();
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

void Eventloop::newconnection(spConnection conn)
{
    std::lock_guard<std::mutex> lock_guard(mmutex_);
    conns_[conn->fd()] = conn;
}

void Eventloop::settimercallback(std::function<void(int)> fn)
{
    timercallback_ = fn;
}
