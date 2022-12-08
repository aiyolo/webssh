#include "Acceptor.h"
#include "InetAddress.h"
#include "util.h"

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
    : loop_(loop), acceptSocket_(), acceptChannel(acceptSocket_.getFd()) {
  acceptSocket_.bind(listenAddr);
}

Acceptor::~Acceptor(){

}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback &cb) {
  newConnectionCallback_ = cb;
}

void Acceptor::listen() { acceptSocket_.listen(); }

void Acceptor::accept() {
  InetAddress peerAddr;
  int connfd = acceptSocket_.accept(&peerAddr);
  if (connfd < 0) {
    LOG << "acceptSocket_.accept eroor...";
  }
  if (newConnectionCallback_) {
    newConnectionCallback_(connfd, peerAddr);
  }
}
