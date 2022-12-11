#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <asm-generic/socket.h>
#include <cstddef>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h> // readv
#include <unistd.h>  //read
#include <netinet/tcp.h> // IPPROTO_TCP 

Socket::Socket() { sockfd_ = socket(AF_INET, SOCK_STREAM, 0); }

void Socket::listen() {
  int ret = ::listen(sockfd_, SOMAXCONN);
  if (ret < 0) {
    LOG << "listen error...";
  }
}

void Socket::bind(const InetAddress &addr) {
  struct sockaddr_in tmpaddr = addr.getAddr();
  int ret = ::bind(sockfd_, (sockaddr *)&tmpaddr, sizeof(tmpaddr));
  if (ret < 0) {
    LOG << "bind error...";
  }
}

// 接受链接，把对端地址写入到 peeraddr
int Socket::accept(InetAddress *peeraddr) {
  int connfd =
      ::accept(sockfd_, (sockaddr *)&peeraddr->addr_, &peeraddr->addrlen_);
  if (connfd < 0) {
    LOG << "accept error...\n";
  }
  return connfd;
}

int Socket::getFd() { return sockfd_; }

int Socket::connect(int sockfd, const struct sockaddr *addr) {
  return ::connect(sockfd, addr,
                   static_cast<socklen_t>(sizeof(struct sockaddr)));
}

ssize_t Socket::read(int sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t Socket::readv(int sockfd, const struct iovec *iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t Socket::write(int sockfd, void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}

ssize_t Socket::writev(int sockfd, const struct iovec *iov, int iovcnt) {
  return ::writev(sockfd, iov, iovcnt);
}

void Socket::close(int sockfd) {
  if (::close(sockfd) < 0) {
    LOG << "close error...";
  }
}

void Socket::shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    LOG << "shutdownWrite error..." << std::endl;
  } 
}

void Socket::setTcpNoDelay()
{
  int optval = 1;
  int ret = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
  if(ret<0){
    LOG << "setTcpNoDelay error..." << std::endl;
  }
}

void Socket::setReuseAddr()
{
  int optval = 1;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
  if(ret<0)
  {
    LOG << "serReuseAddr error...\n" ; 
  }
}

void Socket::setReusePort()
{
  int optval = 1;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
  if(ret<0)
  {
    LOG << "setReusePort error...\n";
  }
}

void Socket::setKeepAlive()
{
  int optval = 1;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET,  SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
  if(ret<0)
  {
    LOG << "setKeepAlive error...\n";
  }
} 