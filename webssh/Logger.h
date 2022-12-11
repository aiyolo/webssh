#pragma once


#include "Buffer.h"
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <thread>
#include <assert.h>

template <class T> class BlockDeque {
public:
  explicit BlockDeque(size_t maxCapacity = 1000);
  ~BlockDeque();
  void clear();
  bool empty();
  bool full();
  void Close();
  size_t size();
  size_t capacity();
  T front();

  T back();

  void push_back(const T &item);

  void push_front(const T &item);

  bool pop(T &item);

  bool pop(T &item, int timeout);

  void flush();

private:
  std::deque<T> deq_;

  size_t capacity_;

  std::mutex mtx_;

  bool isClose_;

  std::condition_variable condConsumer_;

  std::condition_variable condProducer_;
};


class Log {
public:
  void init(int level, const char *path = "./log", const char *suffix = ".log",
            int maxQueueCapacity = 1024);
  static Log *Instance();
  static void flushLogThread();
  void write(int level, const char *format, ...);
  void Flush();
  int getLevel();
  void setLevel(int level);
  bool isOpen() { return isOpen_; };

private:
  Log();
  void appendLogLevelTitle_(int level);
  virtual ~Log();
  void asyncWrite_();

private:
  static const int LOG_PATH_LEN = 256;
  static const int LOG_NAME_LEN = 256;
  static const int MAX_LINES = 50000;

  const char *path_;
  const char *suffix_;

  int MAX_LINES_;

  int lineCount_;
  int toDay_;

  bool isOpen_;
  Buffer buff_;
  int level_;
  bool isAsync_;

  FILE *fp_;

  std::unique_ptr<BlockDeque<std::string>> deque_;
  std::unique_ptr<std::thread> writeThread_;
  std::mutex mtx_;
};
