#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>


#define MAX_EVENTS 1000

Epoll::Epoll() : epfd_(-1) {
  epfd_ = epoll_create(0);
  if (epfd_ < 0) {
    LOG << "epoll_creat error...\n";
  }
  events_ = new epoll_event[MAX_EVENTS];
}

Epoll::~Epoll() {
  if (epfd_ != -1) {
    ::close(epfd_);
    epfd_ = -1;
  }
  delete[] events_;
}

void Epoll::poll(int timeout, std::vector<Channel*> *activeChannels) {
  int nfds = ::epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
  if (nfds < 0) {
    LOG << "epoll_wait error...\n";
  }
  for (int i = 0; i < nfds; i++) {
    Channel *ch = static_cast<Channel *>(events_[i].data.ptr);
    ch->setRevents(events_[i].events);
    activeChannels->emplace_back(ch);
  }
}