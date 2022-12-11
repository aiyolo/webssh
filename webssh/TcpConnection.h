#pragma once
#include <netinet/in.h>
#include <string>
#include "InetAddress.h"
#include "Buffer.h"
#include <memory>

class Channel;
class EventLoop;
class Socket;
class Buffer;
class InetAddress;

class TcpConnection{
public:
    TcpConnection(EventLoop* loop, const std::string& connName, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    std::string getConnName() const;
    void send(const void* data, int len);
    void send(const std::string& message);

    void sendInLoop(const std::string& message);

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    InetAddress getLocalAddr() const ;
    InetAddress getPeerAddr() const ;
    
    EventLoop* loop_;
    const std::string connName_;
    int sockfd_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
     // 可以使用栈变量声明channel，但是这样就不能对Channel 使用前向声明了，这里使用智能指针尝试一下
    std::unique_ptr<Channel> channel_;
};