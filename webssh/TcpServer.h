#pragma once

#include <memory>
#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
class TcpServer {
public:
    TcpServer(EventLoop* loop, const InetAddress & listenAddr);
    ~TcpServer();
    void start();

    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;
};