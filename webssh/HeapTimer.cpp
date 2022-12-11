#include "HeapTimer.h"

// 把节点i上调到一个合适位置
// 节点坐标为i的节点的两个子节点分别为2i+1, 2i+2;
// 因此,父节点总是(i-1)/2
inline void HeapTimer::siftup_(size_t i) {
  assert(i < heap_.size());
  size_t j = (i - 1) / 2;
  while (j < i) {  // Note:
    if (heap_[j] < heap_[i]) {
      break;
    }
    swapNode_(i, j);
    i = j;
    j = (i - 1) / 2;
  }
}
inline void HeapTimer::swapNode_(size_t i, size_t j) {
  std::swap(heap_[i], heap_[j]);
  ref_[heap_[i].id] = i;
  ref_[heap_[j].id] = j;
}

// 如果 返回true, 说明index节点下移了
inline bool HeapTimer::siftdown_(size_t index, size_t n) {
  size_t i = index;
  size_t j = i * 2 + 1;
  while (j < n) {
    // 如果兄弟节点更小,那么使用兄弟节点调换位置
    if (j + 1 < n && heap_[j + 1] < heap_[j])
      j++;
    if (heap_[i] < heap_[j])
      break;
    swapNode_(i, j);
    i = j;
    j = 2 * i + 1;
  }
  return i > index;
}

// 给id节点添加一个延时
inline void HeapTimer::add(int id, int timeout, const TimeoutCallBack &cb) {
  // 如果id节点不存在,那么必定新建在vector尾部,如果timeout比较小,那么可能需要上移
  if (!ref_.count(id)) {
    size_t i = heap_.size();
    ref_[id] = i;
    heap_.push_back({id,
                     std::chrono::high_resolution_clock::now() +
                         std::chrono::milliseconds(timeout),
                     cb});
    siftup_(i);
  }
  // 如果id存在,加入延时后,可能下移,也可能上移
  else {
    size_t i = ref_[id];
    heap_[i].expires = std::chrono::high_resolution_clock::now() +
                       std::chrono::milliseconds(timeout);
    heap_[i].cb = cb;
    if (!siftdown_(i, heap_.size())) { // todo: 不需要siftup
      siftup_(i);
    }
  }
}

// 使id节点工作
// 这里存在冗余, id既为vector坐标,只要直接调id,即可得到对应节点
inline void HeapTimer::doWork(int id) {
  if (heap_.empty() || ref_.count(id) == 0) {
    return;
  }
  size_t i = ref_[id]; // note: heap_[id].id = id;
  TimerNode node = heap_[i];
  node.cb();
  del_(i);
}

// 移除index节点
inline void HeapTimer::del_(size_t index) {
  size_t i = index;
  size_t n = heap_.size() - 1;
  if (i < n) {
    // 把待移除节点与尾部节点交换
    swapNode_(i, n);
    // 交换后节点i是原来的尾部节点,把它放到合适的位置
    if (!siftdown_(i, n)) { // todo: 不需要siftup_
      siftup_(i);
    }
  }
  // 删除节点
  ref_.erase((heap_.back().id));
  heap_.pop_back();
}

inline void HeapTimer::adjust(int id, int timeout) {
  heap_[ref_[id]].expires = std::chrono::high_resolution_clock::now() +
                            std::chrono::milliseconds(timeout);
  siftdown_(ref_[id], heap_.size());
}

inline void HeapTimer::tick() {
  while (!heap_.empty()) {
    TimerNode node = heap_.front();
    // 还没到超时时间
    if (std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() >
        0) {
      break;
    }
    node.cb();
    pop();
  }
}

inline void HeapTimer::pop() { del_(0); }

inline void HeapTimer::clear() {
  ref_.clear();
  heap_.clear();
}

// 触发第一个时钟,并且在第二个时钟已经过期,返回0;
// 如果没有过期返回-1
inline int HeapTimer::getNextTick() {
  tick();
  int res = -1;
  if (!heap_.empty()) {
    res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now())
              .count();
    if (res < 0) {
      res = 0;
    }
  }
  return res;
}