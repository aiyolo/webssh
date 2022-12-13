#include "TcpServer.h"
#include "Buffer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "util.h"

TcpServer::TcpServer(EventLoop* loop, const std::string& name, const InetAddress& listenAddr)
	: loop_(loop)
	, name_(name)
	, connId_(0)
	, acceptor_(new Acceptor(loop, listenAddr))
	, onConnectionCallback_(defaultOnConnectionCallback)
	, onMessageCallback_(defaultOnMessageCallback)
{
	acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
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

	std::string					   connName	 = name_ + "-" + peerAddr.toIpPort() + "#" + std::to_string(connId_);
	InetAddress					   localAddr = acceptor_->getLocalAddr();
	std::unique_ptr<TcpConnection> conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
	conn->setOnConnectionCallback(onConnectionCallback_);
	conn->setOnCloseCallback(onCloseCallback_);
	conn->setOnMessageCallback(onMessageCallback_);
	conn->setOnWriteCompleteCallback(onWriteCompleteCallback_);
	conn->channel_->enableReading();
	connectionMap_[connName] = move(conn);
}
