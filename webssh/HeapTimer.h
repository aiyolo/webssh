#pragma once

#include <assert.h>
#include <chrono>
#include <functional>
#include <vector>

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
  int id;             // node 在vector中的坐标
  TimeStamp expires;  // 过期时间
  TimeoutCallBack cb; // 回调函数
  bool operator<(const TimerNode &node) { return expires < node.expires; }
};

class HeapTimer {
public:
  HeapTimer() { heap_.reserve(64); }
  ~HeapTimer() { clear(); }

  void adjust(int id, int newExpires);
  void add(int id, int timeout, const TimeoutCallBack &cb);
  void doWork(int id);
  void clear();
  void tick();
  void pop();
  int getNextTick();

private:
  void del_(size_t i);
  void siftup_(size_t i);
  bool siftdown_(size_t index, size_t n);
  void swapNode_(size_t i, size_t j);
  std::vector<TimerNode> heap_;
  std::unordered_map<int, size_t> ref_;
};
