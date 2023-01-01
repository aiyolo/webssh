#pragma once

#include <memory>
#include "EventLoop.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include <functional>
#include "Callbacks.h"

class TcpConnection;
class Buffer;

class TcpServer {
public:
    TcpServer(EventLoop* loop, const std::string &name, const InetAddress & listenAddr);
    ~TcpServer();
    void setThreadNum(int numThreads);
    void start();
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn); 
    
    void setOnConnectionCallback(const OnConnectionCallback& cb);
    void setOnMessageCallback(const OnMessageCallback& cb);
    void setOnWriteCompleteCallback(const OnWriteCompleCallback& cb);

    EventLoop* loop_;
    std::string name_;
    int connId_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    OnConnectionCallback onConnectionCallback_;
    OnCloseCallback onCloseCallback_;
    OnMessageCallback onMessageCallback_;
    OnWriteCompleCallback onWriteCompleteCallback_;

    NewConnectionCallback newConnectionCallback_;

    ConnectionMap connectionMap_;
};