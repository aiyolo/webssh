#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include <functional>
#include "InetAddress.h"
#include "Socket.h"

// 负责创建监听socket，并且处理连接来到事件
class Acceptor{
public:
    // NewConnnectionCallback 负责处理每个新创建的连接的事件
    typedef std::function<void(int sockfd, const InetAddress& peerAddr)> NewConnectionCallback;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr);
    ~Acceptor();
    void setNewConnectionCallback(const NewConnectionCallback& cb);
    void accept();
    void listen();

    EventLoop* loop_;
    NewConnectionCallback newConnectionCallback_;
    Socket acceptSocket_;
    Channel acceptChannel;

};