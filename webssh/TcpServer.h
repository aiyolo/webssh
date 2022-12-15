#pragma once

#include <memory>
#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include <functional>
#include "Callbacks.h"

class TcpConnection;
class Buffer;

class TcpServer {
public:
    TcpServer(EventLoop* loop, const std::string &name, const InetAddress & listenAddr);
    ~TcpServer();
    void start();
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnectionInLoop(const TcpConnectionPtr& conn); 
    
    EventLoop* loop_;
    std::string name_;
    int connId_;
    std::unique_ptr<Acceptor> acceptor_;

    OnConnectionCallback onConnectionCallback_;
    OnCloseCallback onCloseCallback_;
    OnMessageCallback onMessageCallback_;
    OnWriteCompleCallback onWriteCompleteCallback_;

    NewConnectionCallback newConnectionCallback_;

    ConnectionMap connectionMap_;
};