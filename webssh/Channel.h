#pragma once
#include <cstdint>
#include <string>
#include <functional>

class EventLoop;
class Epoller;
// Channel 指向fd检测到的事件
class Channel {
public:
  using OnEventCallback = std::function<void()>;
  Channel(EventLoop *loop, int fd);
  ~Channel();
  void enableReading();
  void handleEvent(); // todo 如何处理事件

  void setOnReadEventCallback(OnEventCallback cb);

  void setOnWriteEventCallback(OnEventCallback cb);

  void setOnErrorEventCallback(OnEventCallback cb);

  int getFd() { return fd_; }
  int getIndex() { return index_; }
  void setIndex(int index) { index_ = index; }
  int getEvents() { return events_; }
  int getRevents() { return revents_; }
  void setRevents(uint32_t revents) { revents_ = revents; }
  bool isNoneEvent() const {return events_== kNoneEvent;}


  std::string eventsToString() const {return eventsToString(events_);}
  std::string reventsToString()  const {return eventsToString(revents_);}
  static std::string eventsToString(uint32_t events) ;




  static const uint32_t kNoneEvent;
  static const uint32_t kReadEvent;
  static const uint32_t kWriteEvent;

  EventLoop *loop_;
  int fd_;
  int index_;
  uint32_t events_;
  uint32_t revents_;
  // Epoller* epoller_;
  OnEventCallback onReadEventCallback_;
  OnEventCallback onWriteEventCallback_;
  OnEventCallback onErrorEventCallback_;

};