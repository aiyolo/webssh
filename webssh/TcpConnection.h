#pragma once
#include "Buffer.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include <memory>
#include <netinet/in.h>
#include <string>

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

	std::string getConnName() const;
	void		send(const void* data, int len);
	void		send(const std::string& message);

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

	InetAddress getLocalAddr() const;
	InetAddress getPeerAddr() const;

	void connectionEstablished();
	void connectionDestroyed();

	EventLoop*		  loop_;
	const std::string connName_;
	// int sockfd_;
	std::unique_ptr<Socket>	 socket_;
	std::unique_ptr<Channel> channel_;
	const InetAddress		 localAddr_;
	const InetAddress		 peerAddr_;
	Buffer					 inputBuffer_;
	Buffer					 outputBuffer_;
	// 可以使用栈变量声明channel，但是这样就不能对Channel
	// 使用前向声明了，这里使用智能指针尝试一下

	OnConnectionCallback  onConnectionCallback_;
	OnCloseCallback		  onCloseCallback_;
	OnMessageCallback	  onMessageCallback_;
	OnWriteCompleCallback onWriteCompleteCallback_;
};