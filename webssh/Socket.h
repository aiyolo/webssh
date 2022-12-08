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
  int connect(int sockfd, const struct sockaddr *addr);
  ssize_t read(int sockfd, void *buf, size_t count);
  ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
  ssize_t write(int sockfd, void *buf, size_t count);
  ssize_t writev(int sockfd, const struct iovec *iov, int iovcnt);
  void close(int sockfd);
  void shutdownWrite(int sockfd);

private:
  int sockfd_;
};