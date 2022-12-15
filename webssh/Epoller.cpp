#include "Epoller.h"
#include "Channel.h"
#include "util.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <assert.h>

#define kInitEventListSize 16
#define kNew -1
#define kAdded 1
#define kDeleted 2

Epoller::Epoller()
    : epfd_(epoll_create(EPOLL_CLOEXEC)), events_(kInitEventListSize) {
  if (epfd_ < 0) {
    LOG << "epoll_creat error...\n";
  }
}

Epoller::~Epoller() {
  if (epfd_ != -1) {
    ::close(epfd_);
    epfd_ = -1;
  }
}

void Epoller::updateChannel(Channel* channel)
{
    PrintFuncName pf("Epoller::updatechannel");
    // Poller::assertInLoopThread();
    const int index = channel->getIndex();
    LOG << "fd = " << channel->getFd()
        << " events = " << channel->getEvents() << " index = " << index << std::endl;
    int fd = channel->getFd();
    if (index == kNew || index == kDeleted)
    {
        if (index == kNew)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        // 之前已经添加到map但是没有活跃事件，被标记乘kDeleted的channel，重新活跃起来了，需要重新添加到epfd_实例上
        else // index == kDeleted
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else // index == kAdded
    {
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        // 如果channel上没有事件，在epfd上移除相应的fd
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::removeChannel(Channel *channel)
{
  int fd = channel->getFd();
  int index = channel->getIndex();
  size_t n = channels_.erase(fd);
  assert(n==1);
  if(index==kAdded){
    update(EPOLL_CTL_DEL, channel);
  }
  channel->setIndex(kNew);
}

void Epoller::update(int operation, Channel* channel){
  
  struct epoll_event event;
  memset(&event, 0, sizeof event);
  event.events = channel->getEvents();
  event.data.ptr = channel;
  int fd = channel->getFd();
  int ret = ::epoll_ctl(epfd_, operation, fd, &event);
  if(ret<0){
    LOG << "Epoller::updatedEvents error...";
  }
}

void Epoller::poll(int timeout, std::vector<Channel *> *activeChannels) {
  PrintFuncName pf("Epoller::poll");
  int nfds = ::epoll_wait(epfd_, &*events_.begin(), events_.size(), timeout);
  if (nfds < 0) {
    LOG << "epoll_wait error...\n";
  }
  for (int i = 0; i < nfds; i++) {
    Channel *ch = static_cast<Channel *>(events_[i].data.ptr);
    ch->setRevents(events_[i].events);
    activeChannels->emplace_back(ch);
  }
}


std::string Epoller::opToString(int op)
{
  switch(op)
  {
    case EPOLL_CTL_ADD:
      return "ADD";
    case EPOLL_CTL_DEL:
      return "DEL";
    case EPOLL_CTL_MOD:
      return "MOD";
    default:
      return "Unknown Operation";
  }
}