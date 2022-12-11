#include "Buffer.h"
#include <assert.h>
#include <iostream>
#include <sys/uio.h> // readv
#include <unistd.h>
#include <vector>

// 仿照muduo 预留一部分空间
Buffer::Buffer(size_t initBufferSize)
    : buffer_(initBufferSize), readIndex_(kPrependSize),
      writeIndex_(kPrependSize) {}

size_t Buffer::writableBytes() const { return buffer_.size() - writeIndex_; }

size_t Buffer::readableBytes() const { return writeIndex_ - readIndex_; }

size_t Buffer::prependableBytes() const { return readIndex_; }

const char *Buffer::peek() const { return begin() + readIndex_; }

char *Buffer::begin() { return &*buffer_.begin(); }

const char *Buffer::begin() const { return &*buffer_.begin(); }

void Buffer::retrieve(size_t len) {
  assert(len <= readableBytes());
  if (len <= readableBytes()) {
    readIndex_ += len;
  } else {
    retrieveAll();
  }
}

void Buffer::retrieveUntil(const char *end) {
  assert(peek() <= end);
  assert(end <= beginWrite());
  retrieve(end - peek());
}

void Buffer::retrieveAll() {
  readIndex_ = kPrependSize;
  writeIndex_ = kPrependSize;
}

std::string Buffer::retrieveAllAsString() {
  std::string str(peek(), readableBytes());
  retrieveAll();
  return str;
}

const char *Buffer::beginWriteConst() const { return begin() + writeIndex_; }

char *Buffer::beginWrite() { return begin() + writeIndex_; }

void Buffer::hasWritten(size_t len) {
  assert(len <= writableBytes());
  writeIndex_ += len;
}

void Buffer::append(const std::string &str) { append(str.data(), str.size()); }

void Buffer::append(const char *data, size_t len) {
  ensureWritableBytes(len);
  std::copy(data, data + len, beginWrite());
  hasWritten(len);
}

void Buffer::append(const Buffer &buff) {
  append(buff.peek(), buff.readableBytes());
}

void Buffer::ensureWritableBytes(size_t len) {
  if (writableBytes() < len) {
    makeSpace(len);
  }
  assert(writableBytes() >= len);
}

// 仿照muduo设计
ssize_t Buffer::readFd(int fd, int *savedErrno) {
  char extrabuf[65536];
  struct iovec iov[2];
  size_t writable = writableBytes();

  iov[0].iov_base = begin() + writeIndex_;
  iov[0].iov_len = writable;
  iov[1].iov_base = extrabuf;
  iov[1].iov_len = sizeof extrabuf;

  // 如果buff_的可写空间不足，则读入多出的部分到extrabuf中
  // 否则直接写入到buff_中
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t len = readv(fd, iov, iovcnt);
  // 读出错
  if (len < 0) {
    *savedErrno = errno;
  }
  // buff_空间充足
  else if (static_cast<size_t>(len) <= writable) {
    writeIndex_ += len;
  }
  // buff_空间不足
  else {
    writeIndex_ = buffer_.size();
    append(extrabuf, len - writable); // 将多出的部分写入到buff_中
  }
  return len;
}

ssize_t Buffer::writeFd(int fd, int *savedErrno) {
  size_t readable = readableBytes();
  ssize_t len = write(fd, peek(), readable);
  if (len < 0) {
    *savedErrno = errno;
  } else {
    retrieve(static_cast<size_t>(len));
  }
  return len;
}

void Buffer::makeSpace(size_t len) {
  // 如果可写空间小于len，那么就扩展buff_的大小
  if (writableBytes() + prependableBytes() < len + kPrependSize) {
    buffer_.resize(writeIndex_ + len);
  }
  // 否则，将可读的数据移动到前面，使得移位后的可读空间足够
  else {
    size_t readable = readableBytes();
    std::copy(begin() + readIndex_, begin() + writeIndex_,
              begin() + kPrependSize);
    readIndex_ = kPrependSize;
    writeIndex_ = readIndex_ + readable;
    assert(readable == readableBytes());
  }
}
