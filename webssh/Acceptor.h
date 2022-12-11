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
    void defaultNewConnnectionCallabck(int sockfd, const InetAddress& peerAddr);

    EventLoop* loop_;
    InetAddress localAddr_;
    NewConnectionCallback newConnectionCallback_;
    Socket acceptSocket_;
    Channel acceptChannel;
    // Acceptor在单线程里，不用使用互斥量
    int connId_; 
    // Acceptor 要管理连接，需要保存connection实例，使用map保存connName->connection实例的映射
    // 这就要求在newConnectionCallback_创建的实例conn，在离开函数时也存在，所以需要动态内存管理；
    
};

