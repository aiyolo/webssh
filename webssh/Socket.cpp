#include "Socket.h"
#include "InetAddress.h"
#include <sys/socket.h>
#include "util.h"
#include <cstring>

Socket::Socket(){
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
}

void Socket::listen(){
    int ret = ::listen(sockfd_, SOMAXCONN) ;
    if(ret<0){
        LOG << "listen error...";
    }
}

void Socket::bind(InetAddress* addr){
    struct sockaddr_in tmpaddr = addr->getAddr();
    int ret = ::bind(sockfd_, (sockaddr*)&tmpaddr, sizeof(tmpaddr));
    if(ret<0){
        LOG << "bind error...";
    }
}

// 接受链接，把对端地址写入到 peeraddr
int Socket::accept(InetAddress* peeraddr){
    int connfd = ::accept(sockfd_, (sockaddr*)&peeraddr->addr_, &peeraddr->addrlen_);
    if(connfd<0){
        LOG << "accept error...";
    }
    return connfd;
}

int Socket::getFd(){
    return sockfd_;
}