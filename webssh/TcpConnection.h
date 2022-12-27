#pragma once
#include "Buffer.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include <cstddef>
#include <memory>
#include <netinet/in.h>
#include <string>
#include "HttpContext.h"

class Channel;
class EventLoop;
class Socket;
class Buffer;
class InetAddress;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(EventLoop* loop, const std::string& connName, int sockfd, const InetAddress& localAddr,
				  const InetAddress& peerAddr);
	~TcpConnection();

	void shutdown();
	void shutdownInLoop();
	void setState(ConnState state);

	std::string getConnName() const;
	void		send(const void* data, int len);
	void		send(const std::string& message);
	void send(Buffer* buff);
	
	void sendInLoop(const std::string& message);
	void sendInLoop(const void* data, size_t len);

	void handleRead();
	void handleWrite();
	void handleClose();
	void handleError();

	void connectDestroyed();
	void setOnConnectionCallback(const OnConnectionCallback& cb);
	void setOnMessageCallback(const OnMessageCallback& cb);
	void setOnWriteCompleteCallback(const OnWriteCompleCallback& cb);
	void setOnCloseCallback(const OnCloseCallback& cb);
	void setHighWaterMarkCallback_(const HighWaterMarkCallback& cb, size_t highWaterMark);
	InetAddress getLocalAddr() const;
	InetAddress getPeerAddr() const;

	void connectionEstablished();
	void connectionDestroyed();

	void setContext(const HttpContext& context);
	const HttpContext& getContext() const;
	EventLoop*		  loop_;
	const std::string connName_;
	ConnState state_;
	// int sockfd_;
	std::unique_ptr<Socket>	 socket_;
	std::unique_ptr<Channel> channel_;
	const InetAddress		 localAddr_;
	const InetAddress		 peerAddr_;
	Buffer					 inputBuffer_;
	Buffer					 outputBuffer_;
	size_t highWaterMark_;
	// 可以使用栈变量声明channel，但是这样就不能对Channel
	// 使用前向声明了，这里使用智能指针尝试一下

	HttpContext context_;

	OnConnectionCallback  onConnectionCallback_;
	OnCloseCallback		  onCloseCallback_;
	OnMessageCallback	  onMessageCallback_;
	OnWriteCompleCallback onWriteCompleteCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;
};