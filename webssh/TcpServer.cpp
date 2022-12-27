#include "TcpServer.h"
#include "Buffer.h"
#include "Callbacks.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "util.h"
#include <assert.h>

TcpServer::TcpServer(EventLoop* loop, const std::string& name, const InetAddress& listenAddr)
	: loop_(loop)
	, name_(name)
	, connId_(0)
	, acceptor_(new Acceptor(loop, listenAddr))
	, onConnectionCallback_(defaultOnConnectionCallback)
	, onMessageCallback_(defaultOnMessageCallback)
{
	acceptor_->setNewConnectionCallback(
		std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{}

void TcpServer::start()
{
	acceptor_->listen();
	//   loop_->runInLoop(&acceptor_::listen());
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	loop_->assertInLoopThread();
	std::string					   connName	 = name_ + "-" + peerAddr.toIpPort() + "#" + std::to_string(connId_);
	InetAddress					   localAddr = acceptor_->getLocalAddr();
	TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
	connectionMap_[connName] = conn;
	conn->setOnConnectionCallback(onConnectionCallback_);
	conn->setOnCloseCallback(std::bind(&TcpServer::removeConnectionInLoop, this, std::placeholders::_1));
	conn->setOnMessageCallback(onMessageCallback_);
	conn->setOnWriteCompleteCallback(onWriteCompleteCallback_);
	loop_->runInLoop(std::bind(&TcpConnection::connectionEstablished, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	size_t n = connectionMap_.erase(conn->getConnName());
	assert(n == 1);
	conn->connectDestroyed();
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