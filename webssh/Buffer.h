#pragma once
#include <cstddef>
#include <string>
#include <vector>

class Buffer {
public:
  static const size_t kPrependSize = 8;
  static const size_t kInitialSize = 1024;
  Buffer(size_t initBufferSize = kInitialSize);
  ~Buffer() = default;

  size_t size() const { return buffer_.size(); }
  size_t writableBytes() const;
  size_t readableBytes() const;
  size_t prependableBytes() const;

  const char *peek() const;
  void retrieve(size_t len);
  void retrieveUntil(const char *end);
  void retrieveAll();
  std::string retrieveAsString(size_t len);
  std::string retrieveAllAsString();

  const char *beginWriteConst() const;
  char *beginWrite();

  void append(const char *data, size_t len);
  void append(const std::string &str);
  void append(const Buffer &buffer);

  void prepend(const char *data, size_t len);
  void prepend(const std::string &str);
  void shrink(size_t reserve);
  void ensureWritableBytes(size_t len);
  void swap(Buffer &rhs);
  void hasWritten(size_t len);

  const char *findCRLF() const;
  const char *findCRLF(const char *start) const;
  const char *findEOL() const;
  const char *findEOL(const char *start) const;
  const char *findEOL(const char *start, const char *end) const;
  const char *findEOL(const char *start, const char *end,
                      const char *eol) const;

  ssize_t readFd(int fd, int *savedErrno);
  ssize_t writeFd(int fd, int *savedErrno);

  void makeSpace(size_t len);
  char *begin();
  const char *begin() const;

private:
  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;
};
