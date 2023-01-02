#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// 配置阶段，不要执行启动的操作
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
	: loop_(loop)
	, localAddr_(listenAddr)
	// , newConnectionCallback_(defaultNewConnnectionCallback)
	, acceptSocket_()
	, acceptChannel(loop, acceptSocket_.getFd())
  , idleFd_(::open("/dev/null", O_RDONLY|O_CLOEXEC))
{
	// LOG << "server addr:" << listenAddr.toIpPort() << std::endl;
	acceptSocket_.setReuseAddr();
	acceptSocket_.setReusePort();
	acceptSocket_.setNonBlocking();
	acceptSocket_.bind(listenAddr);
	// acceptsocket上有连接时，触发读事件，使用accept方法处理
	acceptChannel.setOnReadEventCallback(std::bind(&Acceptor::accept, this));
}

Acceptor::~Acceptor()
{}

// connection类负责管理连接建立、消息到来，消息发送、关闭连接事件发生时的行为，不去操心收发数据，这几种发生行为在应用层，需由用户指定针对不同的事件采取什么方式，Acceptor->TcpServer，用户可以在TcpServer中指定newConnectionBack,因此具体实现交给TcpServer实现
void Acceptor::setNewConnectionCallback(const NewConnectionCallback& cb)
{
	newConnectionCallback_ = cb;
}

void Acceptor::listen()
{
	acceptSocket_.listen();
	acceptChannel.enableReading();	 // 监听 acceptSocket_上的读事件；
}

void Acceptor::accept()
{
	InetAddress peerAddr;
	int			connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0) {
		if (newConnectionCallback_) {
			newConnectionCallback_(connfd, peerAddr);
		}
		else {
			LOG << "acceptSocket_.accept error..." << std::strerror(errno) << std::endl;
			Socket::close(connfd);
		}
	}
	else {
    // 如果没有空闲的fd，那么关闭dummy，用dummy接受新连接，再关闭新连接，再重新创建dummy fd
		if (errno == EMFILE) {
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.getFd(), NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}

InetAddress Acceptor::getLocalAddr()
{
	return localAddr_;
}

void Acceptor::defaultNewConnnectionCallback(int sockfd, const InetAddress& peerAddr)
{
	// PrintFuncName pf("defaultNewConnectionCallback");
	// std::string connName = std::to_string(connId_);
	// TcpConnection conn(loop_, connName, sockfd, localAddr_, peerAddr);
	// LOG << "setup connection manager:" << peerAddr.toIpPort() << " with sockfd " << sockfd << std::endl;
}