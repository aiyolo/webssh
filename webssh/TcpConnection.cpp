#include "TcpConnection.h"

#include "Callbacks.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include <sys/types.h>

TcpConnection::TcpConnection(EventLoop* loop, const std::string& connName, int sockfd, const InetAddress& localAddr,
							 const InetAddress& peerAddr)
	: loop_(loop)
	, connName_(connName)
	, socket_(new Socket(sockfd))
	, channel_(new Channel(loop, sockfd))
	, localAddr_(localAddr)
	, peerAddr_(peerAddr)
{
	channel_->setOnReadEventCallback(std::bind(&TcpConnection::handleRead, this));
	channel_->setOnWriteEventCallback(std::bind(&TcpConnection::handleWrite, this));
	channel_->setOnErrorEventCallback(std::bind(&TcpConnection::handleError, this));
	channel_->setOnCloseEventCallback(std::bind(&TcpConnection::handleClose, this));
}

TcpConnection::~TcpConnection()
{}

std::string TcpConnection::getConnName() const
{
	return connName_;
}
void TcpConnection::send(const void* data, int len)
{
	const char* ch = static_cast<const char*>(data);
	std::string message;
	message.assign(ch, ch + len);
	sendInLoop(message);
}

void TcpConnection::send(const std::string& message) 
{
	sendInLoop(message);
}

void TcpConnection::sendInLoop(const std::string& message) 
{
	LOG << message << std::endl;
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len) 
{
    // const void* -> const T* 需要static_cast;
    outputBuffer_.append(static_cast<const char*>(data), len);
    if(!channel_->isWriting())
    {
        channel_->enableWriting();
    }
}

void TcpConnection::handleRead()
{
	PrintFuncName pf("TcpConnection::handleRead...");
	int			  savedErrno = 0;
	ssize_t		  n			 = inputBuffer_.readFd(channel_->getFd(), &savedErrno);
	if (n > 0) {
		onMessageCallback_(shared_from_this(), &inputBuffer_);
	}
	else if (n == 0) {
		handleClose();
	}
	else {
		errno = savedErrno;
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	PrintFuncName pf("TcpConnection handleWrite...\n");
	if (channel_->isWriting()) {
		ssize_t n = socket_->write(channel_->getFd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
		if (n > 0) {
			outputBuffer_.retrieve(n);
            // 如果数据全部写完了，取消注册channel的写通道
			if (outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();
			}
		}
	}
}

void TcpConnection::handleClose()
{
	PrintFuncName pf("TcpConnection handleClose...\n");
	channel_->disableAll();
	TcpConnectionPtr guardThis(shared_from_this());
	onCloseCallback_(guardThis);
}

void TcpConnection::handleError()
{
	PrintFuncName pf("TcpConnection handleError...\n");
}

void TcpConnection::connectDestroyed()
{
	channel_->disableAll();
	channel_->remove();
}

void TcpConnection::setOnConnectionCallback(const OnConnectionCallback& cb)
{
	onConnectionCallback_ = cb;
}

void TcpConnection::setOnCloseCallback(const OnCloseCallback& cb)
{
	onCloseCallback_ = cb;
}

void TcpConnection::setOnMessageCallback(const OnMessageCallback& cb)
{
	onMessageCallback_ = cb;
}

void TcpConnection::setOnWriteCompleteCallback(const OnWriteCompleCallback& cb)
{
	onWriteCompleteCallback_ = cb;
}
InetAddress TcpConnection::getLocalAddr() const
{
	return localAddr_;
}

InetAddress TcpConnection::getPeerAddr() const
{
	return peerAddr_;
}

void TcpConnection::connectionEstablished()
{
	channel_->tie(shared_from_this());
	channel_->enableReading();
}