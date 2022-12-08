#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress &listenAddr):loop_(loop),acceptor_(new Acceptor(loop, listenAddr)){

}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    // loop_->runInLoop(&acceptor_::listen());
}