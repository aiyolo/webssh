#include "Channel.h"
#include "Epoller.h"
#include "EventLoop.h"
#include "util.h"
#include <sys/epoll.h>

const uint32_t Channel::kNoneEvent = 0;
const uint32_t Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const uint32_t Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), index_(-1), events_(0), revents_(0) {}

Channel::~Channel() {}

void Channel::enableReading() {
  events_ = EPOLLIN | EPOLLET;
  /* 现在要把events的修改回传到 epoller_, 需要执行epoller_.update(events_,
   * channel);
   * 因此channel需要拥有epoller_指针，但是分析类之间的关系，可以发现acceptChannel是在Acceptor的构造函数里创建的，外部可见变量只有loop,而epoller是在EventLoop类里创建的，所以存在依赖链channel->epoller->loop,所以让channel直接关联EventLoop类，即使用loop_指针指向EventLoop类;
   */
  loop_->updateChannel(this);
}

void Channel::handleEvent() {
  PrintFuncName pf("Channel::handleEvent");
  LOG << eventsToString() << std::endl;
  if(revents_&(EPOLLERR))
  {
    if(onErrorEventCallback_) onErrorEventCallback_();
  }
  if(revents_ &(EPOLLIN | EPOLLPRI | EPOLLRDHUP))
  {
    if(onReadEventCallback_) onReadEventCallback_();
  }
  if(revents_ &(EPOLLOUT))
  {
    if(onWriteEventCallback_) onWriteEventCallback_();
  }
}

void Channel::setOnReadEventCallback(OnEventCallback cb){
  onReadEventCallback_ = cb;
}

void Channel::setOnWriteEventCallback(OnEventCallback cb)
{
  onWriteEventCallback_ = cb;
}

void Channel::setOnErrorEventCallback(OnEventCallback cb)
{
  onErrorEventCallback_ = cb;
}


std::string Channel::eventsToString(uint32_t events)  {
  std::string str;
  if (events & EPOLLIN) {
    str += "EPOLLIN ";
  }
  if (events & EPOLLPRI) {
    str += "EPOLLPRI ";
  }
  if (events & EPOLLOUT) {
    str += "EPOLLOUT ";
  }
  if (events & EPOLLHUP) {
    str += "EPOLLHUP ";
  }
  if (events & EPOLLRDHUP) {
    str += "EPOLLRDHUP ";
  }
  if (events & EPOLLERR) {
    str += "EPOLLERR ";
  }
  if (events & EPOLLONESHOT) {
    str += "EPOLLONESHOT ";
  }
  // if(events & EPOLLPRI){
  //     str += "EPOLLPRI";
  // }
  if (events & EPOLLET) {
    str += "EPOLLET ";
  }
  return str;
}
