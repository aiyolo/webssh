#include "Logger.h"
#include <stdarg.h> // va_start va_end
#include "util.h"

template <class T>
BlockDeque<T>::BlockDeque(size_t maxCapacity) : capacity_(maxCapacity) {
  assert(maxCapacity > 0);
  isClose_ = false;
}

template <class T> BlockDeque<T>::~BlockDeque() { Close(); }

template <class T> void BlockDeque<T>::Close() {
  {
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
    isClose_ = true;
  }
  condProducer_.notify_all();
  condConsumer_.notify_all();
}

template <class T> void BlockDeque<T>::flush() { condConsumer_.notify_one(); };

template <class T> void BlockDeque<T>::clear() {
  std::lock_guard<std::mutex> locker(mtx_);
  deq_.clear();
}

template <class T> T BlockDeque<T>::front() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.front();
}

template <class T> T BlockDeque<T>::back() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.back();
}

template <class T> size_t BlockDeque<T>::size() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.size();
}

template <class T> size_t BlockDeque<T>::capacity() {
  std::lock_guard<std::mutex> locker(mtx_);
  return capacity_;
}

template <class T> void BlockDeque<T>::push_back(const T &item) {
  std::unique_lock<std::mutex> locker(mtx_);
  // 如果当前队列满了,生产者需要等待消费者消费完,才能push
  while (deq_.size() >= capacity_) {
    condProducer_.wait(locker);
  }
  deq_.push_back(item);
  // 通知消费者
  condConsumer_.notify_one();
}

template <class T> void BlockDeque<T>::push_front(const T &item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.size() >= capacity_) {
    condProducer_.wait(locker);
  }
  deq_.push_front(item);
  condConsumer_.notify_one();
}

template <class T> bool BlockDeque<T>::empty() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.empty();
}

template <class T> bool BlockDeque<T>::full() {
  std::lock_guard<std::mutex> locker(mtx_);
  return deq_.size() >= capacity_;
}

template <class T> bool BlockDeque<T>::pop(T &item) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.empty()) {
    condConsumer_.wait(locker);
    if (isClose_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  condProducer_.notify_one();
  return true;
}

template <class T> bool BlockDeque<T>::pop(T &item, int timeout) {
  std::unique_lock<std::mutex> locker(mtx_);
  while (deq_.empty()) {
    if (condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) ==
        std::cv_status::timeout) {
      return false;
    }
    if (isClose_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  condProducer_.notify_one();
  return true;
}

Logger::Logger() {
  lineCount_ = 0;
  isAsync_ = false;
  writeThread_ = nullptr;
  deque_ = nullptr;
  toDay_ = 0;
  fp_ = nullptr;
}

Logger::~Logger() {
  PrintFuncName pf("Logger::~Logger");
  if(writeThread_ && writeThread_->joinable()) {
    while (!deque_->empty()) {
      deque_->flush();
    };
    deque_->Close();
    writeThread_->join();
  }
  if (fp_) {
    std::lock_guard<std::mutex> locker(mtx_);
    Flush();
    fclose(fp_);
  }
}

int Logger::getLevel() {
  std::lock_guard<std::mutex> locker(mtx_);
  return level_;
}

void Logger::setLevel(int level) {
  std::lock_guard<std::mutex> locker(mtx_);
  level_ = level;
}

void Logger::init(int level = 1, const char *path, const char *suffix,
               int maxQueueSize) {
  isOpen_ = true;
  level_ = level;
  if (maxQueueSize > 0) {
    // 设成异步模式
    isAsync_ = true;
    if (!deque_) {
      // 创建阻塞队列和写线程
      std::unique_ptr<BlockDeque<std::string>> newDeque(
          new BlockDeque<std::string>);
      deque_ = move(newDeque);

      std::unique_ptr<std::thread> NewThread(new std::thread(flushLogThread));
      writeThread_ = move(NewThread);
    }
  } else {
    isAsync_ = false;
  }

  lineCount_ = 0;

  // 创建日志文件
  time_t timer = time(nullptr);
  struct tm *sysTime = localtime(&timer);
  struct tm t = *sysTime;
  path_ = path;
  suffix_ = suffix;
  char fileName[LOG_NAME_LEN] = {0};
  snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", path_,
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
  toDay_ = t.tm_mday;

  {
    std::lock_guard<std::mutex> locker(mtx_);
    buff_.retrieveAll();
    if (fp_) {
      Flush();
      fclose(fp_);
    }

    fp_ = fopen(fileName, "a");
    if (fp_ == nullptr) {
      mkdir(path_, 0777);
      fp_ = fopen(fileName, "a");
    }
    assert(fp_ != nullptr);
  }
}

void Logger::write(int level, const char *format, ...) {
  struct timeval now = {0, 0};
  gettimeofday(&now, nullptr);
  time_t tSec = now.tv_sec;
  struct tm *sysTime = localtime(&tSec);
  struct tm t = *sysTime;

  /* 日志日期 日志行数 */
  if (toDay_ != t.tm_mday || (lineCount_ && (lineCount_ % MAX_LINES == 0))) {
    std::unique_lock<std::mutex> locker(mtx_);
    locker.unlock();

    char newFile[LOG_NAME_LEN];
    char tail[36] = {0};
    snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1,
             t.tm_mday);

    if (toDay_ != t.tm_mday) {
      snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path_, tail, suffix_);
      toDay_ = t.tm_mday;
      lineCount_ = 0;
    } else {
      snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_, tail,
               (lineCount_ / MAX_LINES), suffix_);
    }

    locker.lock();
    Flush();
    fclose(fp_);
    fp_ = fopen(newFile, "a");
    assert(fp_ != nullptr);
  }

  {
    std::unique_lock<std::mutex> locker(mtx_);
    lineCount_++;
    // 向buff写入当前时间
    int n =
        snprintf(buff_.beginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                 t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
                 t.tm_sec, now.tv_usec);
    // 指针后移n字节
    buff_.hasWritten(n);
    // 写入 [info: ]
    appendLogLevelTitle_(level);

    // 可变参数宏，把可能出现的多个参数 format 字符串写入buff.writable
    va_list vaList;
    va_start(vaList, format);
    int m =
        vsnprintf(buff_.beginWrite(), buff_.writableBytes(), format, vaList);
    va_end(vaList);

    buff_.hasWritten(m);
    buff_.append("\n\0", 2);
    if (isAsync_ && deque_ && !deque_->full()) {
      deque_->push_back(buff_.retrieveAllAsString());
    } else {
      fputs(buff_.peek(), fp_);
    }
    buff_.retrieveAll();
  }
}

void Logger::appendLogLevelTitle_(int level) {
  switch (level) {
  case 0:
    buff_.append("[debug]: ", 9);
    break;
  case 1:
    buff_.append("[info] : ", 9);
    break;
  case 2:
    buff_.append("[warn] : ", 9);
    break;
  case 3:
    buff_.append("[error]: ", 9);
    break;
  default:
    buff_.append("[info] : ", 9);
    break;
  }
}

void Logger::Flush() {
  if (isAsync_) {
    deque_->flush();
  }
  fflush(fp_);
}

void Logger::asyncWrite_() {
  std::string str = "";
  // log init时创建的线程,一直在等待生产者投喂
  while (deque_->pop(str)) {
    std::lock_guard<std::mutex> locker(mtx_);
    fputs(str.c_str(), fp_);
  }
}

Logger *Logger::Instance() {
  static Logger inst;
  return &inst;
}

void Logger::flushLogThread() { Logger::Instance()->asyncWrite_(); }