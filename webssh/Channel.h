#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <functional>
#include "Callbacks.h"

class EventLoop;
class Epoller;
// Channel 指向fd检测到的事件
class Channel {
public:
  Channel(EventLoop *loop, int fd);
  ~Channel();
  void tie(const std::shared_ptr<void>& sp);
  void enableReading();
  void disableReading();
  void enableWriting();
  void disableWriting();
  void disableAll();
  bool isNoneEvent();
  bool isReading();
  bool isWriting();
  void handleEvent(); // todo 如何处理事件
  void handleEventWithGuard();

  void setOnReadEventCallback(const OnReadEventCallback& cb);

  void setOnWriteEventCallback(const OnWriteEventCallback& cb);

  void setOnErrorEventCallback(const OnErrorEventCallback& cb);
  
  void setOnCloseEventCallback(const OnCloseEventCallback& cb);

  int getFd() { return fd_; }
  int getIndex() { return index_; }
  void setIndex(int index) { index_ = index; }
  int getEvents() { return events_; }
  int getRevents() { return revents_; }
  void setRevents(uint32_t revents) ;
  bool isNoneEvent() const {return events_== kNoneEvent;}


  std::string eventsToString() const;

  std::string reventsToString() const;

  static std::string eventsToString(uint32_t events) ;
  void remove();
  void removeChannel();
  
  static const uint32_t kNoneEvent;
  static const uint32_t kReadEvent;
  static const uint32_t kWriteEvent;

  EventLoop *loop_;
  int fd_;
  int index_;
  uint32_t events_;
  uint32_t revents_;
  // Epoller* epoller_;
  std::weak_ptr<void> tie_;
  bool tied_;

  OnReadEventCallback onReadEventCallback_;
  OnWriteEventCallback onWriteEventCallback_;
  OnErrorEventCallback onErrorEventCallback_;
  OnCloseEventCallback onCloseEventCallback_;

};