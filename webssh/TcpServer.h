#pragma once

#include <memory>
#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include <functional>

class TcpConnection;
class Buffer;

class TcpServer {
public:
    using OnConnectionCallback = std::function<void(const TcpConnection*)>;
    using OnMessageCallback = std::function<void(const TcpConnection*, Buffer*)>;
    using OnCloseCallback = std::function<void(const TcpConnection*)>;
    using OnWriteCompleCallback = std::function<void(const TcpConnection*)>;

    TcpServer(EventLoop* loop, const InetAddress & listenAddr);
    ~TcpServer();
    void start();
    void newConnection(int sockfd, const InetAddress& peerAddr);
    
    static void defaultOnConnectionCallback(const TcpConnection* conn);
    static void defaultOnMessageCallback(const TcpConnection* conn, Buffer* buffer);

    
    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;
    OnCloseCallback onConnectionCallback_;
    OnMessageCallback onMessageCallback_;
    OnWriteCompleCallback onWriteCompleteCallback_;

};