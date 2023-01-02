#include "TcpConnection.h"
#include "Callbacks.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include <asm-generic/errno.h>
#include <assert.h>
#include <cstring>
#include <sys/types.h>


TcpConnection::TcpConnection(EventLoop* loop, const std::string& connName, int connId, int sockfd, const InetAddress& localAddr,
							 const InetAddress& peerAddr)
	: loop_(loop)
	, connName_(connName)
	, connId_(connId)
	, state_(ConnState::kConnecting)
	, socket_(new Socket(sockfd))
	, channel_(new Channel(loop, sockfd))
	, localAddr_(localAddr)
	, peerAddr_(peerAddr)
	, highWaterMark_(64 * 1024 * 1024)
{
	// LOG << "new connId_:" << connId_ << std::endl;
	channel_->setOnReadEventCallback(std::bind(&TcpConnection::handleRead, this));
	channel_->setOnWriteEventCallback(std::bind(&TcpConnection::handleWrite, this));
	channel_->setOnErrorEventCallback(std::bind(&TcpConnection::handleError, this));
	channel_->setOnCloseEventCallback(std::bind(&TcpConnection::handleClose, this));
}

TcpConnection::~TcpConnection()
{
	// LOG << "delete connId_" << connId_ << std::endl;
}

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
	// LOG << message << std::endl;
	sendInLoop(message.data(), message.size());
}

// 直接在本线程发送
void TcpConnection::sendInLoop(const void* data, size_t len)
{
	loop_->assertInLoopThread();
	ssize_t nwrote	  = 0;
	size_t	remaining = len;
	if (state_ == ConnState::kDisconnected) {
		// LOG << "connection is disconnected...\n";
	}
	// FIXME if是否可以省略？如果outputBuffer还有数据，那么要先发送outputBuffer之后再发送data
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		// 尝试直接发送数据
		nwrote = Socket::write(channel_->getFd(), data, len);
		if (nwrote >= 0) {
			remaining = len - nwrote;
			if (remaining == 0 && onWriteCompleteCallback_) {
				loop_->runInLoop(std::bind(onWriteCompleteCallback_, shared_from_this()));
			}
		}
		else {
			nwrote = 0;
			if (errno != EWOULDBLOCK) {
				// LOG << "error ocurrs...\n";
			}
		}
		//
		if (remaining > 0) {
			size_t oldLen = outputBuffer_.readableBytes();
			if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ && highWaterMarkCallback_) {
				loop_->runInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
			}
			// 把剩余没法送完的数据，存到outputBuffer
			outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
			if (!channel_->isWriting()) {
				channel_->enableWriting();
			}
		}
	}
}

void TcpConnection::handleRead()
{
	loop_->assertInLoopThread();
	int			  savedErrno = 0;
	ssize_t		  n			 = inputBuffer_.readFd(channel_->getFd(), &savedErrno);
	if (n > 0) {
		if (onMessageCallback_) {
			onMessageCallback_(shared_from_this(), &inputBuffer_);
		}
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
	loop_->assertInLoopThread();
	if (channel_->isWriting()) {
		ssize_t n = socket_->write(channel_->getFd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
		if (n > 0) {
			outputBuffer_.retrieve(n);
			// 如果数据全部写完了，取消注册channel的写通道
			if (outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();
			}
			if(onWriteCompleteCallback_)
			{
				loop_->runInLoop(std::bind(onWriteCompleteCallback_, shared_from_this()));
			}
			if(state_==ConnState::kDisconnecting)
			{
				shutdownInLoop();
			}
		}
	}
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	assert(state_==ConnState::kConnected || state_==ConnState::kDisconnecting);
	setState(ConnState::kDisconnected);
	channel_->disableAll();
	TcpConnectionPtr guardThis(shared_from_this());
	onConnectionCallback_(guardThis);
	onCloseCallback_(guardThis);
}

void TcpConnection::handleError()
{
	// int err = Socket::getSocketError(channel_->getFd());
  	// LOG << "TcpConnection::handleError []" << "- SO_ERROR = " << err << " " << std::strerror(err);
}

void TcpConnection::connectEstablished()
{
	loop_->assertInLoopThread();
	assert(state_ == ConnState::kConnecting);
	setState(ConnState::kConnected);
	channel_->tie(shared_from_this());
	channel_->enableReading();
	onConnectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
	loop_->assertInLoopThread();
	if(state_==ConnState::kConnected)
	{
		setState(ConnState::kDisconnected);
		channel_->disableAll();
		onConnectionCallback_(shared_from_this());

	}
	channel_->remove();
}

EventLoop* TcpConnection::getLoop() const
{
	return loop_;
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

void TcpConnection::setHighWaterMarkCallback_(const HighWaterMarkCallback& cb, size_t highWaterMark)
{
	highWaterMarkCallback_ = cb;
	highWaterMark_		   = highWaterMark;
}
InetAddress TcpConnection::getLocalAddr() const
{
	return localAddr_;
}

InetAddress TcpConnection::getPeerAddr() const
{
	return peerAddr_;
}

void TcpConnection::setContext(const HttpContext& context)
{
	context_ = context;
}

const HttpContext& TcpConnection::getContext() const
{
	return context_;
}



void TcpConnection::send(Buffer* buff)
{
	if (state_ == ConnState::kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(buff->peek(), buff->readableBytes());
			buff->retrieveAll();
		}
		else {
			void (TcpConnection::*fp)(const std::string& message) = &TcpConnection::sendInLoop;
			loop_->runInLoop(std::bind(fp, this, buff->retrieveAllAsString()));
		}
	}
}

void TcpConnection::setState(ConnState state)
{
	state_ = state;
}

void TcpConnection::shutdown()
{
	if (state_ == ConnState::kConnected) {
		setState(ConnState::kDisconnecting);
		// 为什么是shutdownInLoop;
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if (!channel_->isWriting()) {
		socket_->shutdownWrite();
	}
}