#include "TcpServer.h"
#include "Buffer.h"
#include "Callbacks.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "util.h"
#include <assert.h>

TcpServer::TcpServer(EventLoop* loop, const std::string& name, const InetAddress& listenAddr)
	: loop_(loop)
	, name_(name)
	, connId_(0)
	, acceptor_(new Acceptor(loop, listenAddr))
	, threadPool_(new EventLoopThreadPool(loop))
	, onConnectionCallback_(defaultOnConnectionCallback)
	, onMessageCallback_(defaultOnMessageCallback)
{
	acceptor_->setNewConnectionCallback(
		std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{}

void TcpServer::setThreadNum(int numThreads)
{
	threadPool_->setThreadNum(numThreads);
}
void TcpServer::start()
{
	threadPool_->start();
	acceptor_->listen();
	//   loop_->runInLoop(&acceptor_::listen());
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	loop_->assertInLoopThread();
	EventLoop* ioLoop = threadPool_->getNextLoop();
	std::string					   connName	 = name_ + "-" + peerAddr.toIpPort() + "#" + std::to_string(connId_);
	InetAddress					   localAddr = acceptor_->getLocalAddr();
	TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, connId_, sockfd, localAddr, peerAddr));
	connId_++;
	connectionMap_[connName] = conn;
	conn->setOnConnectionCallback(onConnectionCallback_);
	conn->setOnMessageCallback(onMessageCallback_);
	conn->setOnWriteCompleteCallback(onWriteCompleteCallback_);
	conn->setOnCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();
	LOG << "TcpServer::removeConnectionInLoop [" << name_
           << "] - connection " << conn->getConnName();
	size_t n = connectionMap_.erase(conn->getConnName());
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void TcpServer::setOnConnectionCallback(const OnConnectionCallback &cb)
{
	onConnectionCallback_ = cb;
}

void TcpServer::setOnMessageCallback(const OnMessageCallback &cb)
{
	onMessageCallback_ = cb;
}

void TcpServer::setOnWriteCompleteCallback(const OnWriteCompleCallback &cb)
{
	onWriteCompleteCallback_ = cb;
}