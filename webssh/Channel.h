#pragma once
#include <cstdint>

// Channel 指向fd检测到的事件
class Channel {
public:
  Channel(int fd);
  ~Channel();
  void enableReading();
  void setRevents(uint32_t revt);
  void handleEvent(); // todo 如何处理事件
  int getFd();
  int getEvents();
  int getRevents();

  int fd_;
  uint32_t events_;
  uint32_t revents_;
};