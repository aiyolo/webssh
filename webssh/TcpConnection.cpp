#include "TcpConnection.h"

#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include "Channel.h"

TcpConnection::TcpConnection(EventLoop* loop, const std::string& connName, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr):loop_(loop),connName_(connName),sockfd_(sockfd),localAddr_(localAddr),peerAddr_(peerAddr),channel_(new Channel(loop, sockfd))
{
    channel_->setOnReadEventCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setOnWriteEventCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setOnErrorEventCallback(std::bind(&TcpConnection::handleError, this));

}
std::string TcpConnection::getConnName() const {
    return connName_;
}
void TcpConnection::send(const void *data, int len) {
  const char *ch = static_cast<const char *>(data);
  std::string message;
  message.assign(ch, ch + len);
  sendInLoop(message);
}

void TcpConnection::send(const std::string &message) { sendInLoop(message); }

void TcpConnection::sendInLoop(const std::string &message) {
  LOG << message << std::endl;
}

void TcpConnection::handleRead()
{
    PrintFuncName pf("TcpConnection handleRead...\n");
}

void TcpConnection::handleWrite()
{
    PrintFuncName pf("TcpConnection handleWrite...\n");
}


void TcpConnection::handleClose()
{
    PrintFuncName pf("TcpConnection handleClose...\n");
}

void TcpConnection::handleError()
{
    PrintFuncName pf("TcpConnection handleError...\n");
}

InetAddress TcpConnection::getLocalAddr() const { return localAddr_; }

InetAddress TcpConnection::getPeerAddr() const { return peerAddr_; }