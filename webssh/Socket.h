#pragma once

#include <bits/types/struct_iovec.h>
#include <sys/types.h>


class InetAddress;

class Socket {
public:
  Socket(int fd) : sockfd_(fd) {}
  Socket();
  void listen();
  void bind(const InetAddress &addr);
  int accept(InetAddress *peeraddr);
  int getFd();
  /// connect函数等等操作sockfd的函数，与Socket类是松耦合的，可以独立出去，他们不直接操作Socket的成员变量，在muduo里，这些函数放在sockets命令空间里
  int connect(int sockfd, const struct sockaddr *addr);
  ssize_t read(int sockfd, void *buf, size_t count);
  ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
  ssize_t write(int sockfd, void *buf, size_t count);
  ssize_t writev(int sockfd, const struct iovec *iov, int iovcnt);
  void close(int sockfd);
  void shutdownWrite(int sockfd);

  void setTcpNoDelay();
  void setReuseAddr();
  void setReusePort();
  void setKeepAlive();

private:
  int sockfd_;
};