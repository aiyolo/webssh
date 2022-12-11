#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "util.h"
#include "TcpConnection.h"
#include "Buffer.h"

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
    : loop_(loop), acceptor_(new Acceptor(loop, listenAddr)), onConnectionCallback_(defaultOnConnectionCallback), onMessageCallback_(defaultOnMessageCallback){
        // acceptor_->
    }

TcpServer::~TcpServer() {}

void TcpServer::start() {
    acceptor_->listen();
//   loop_->runInLoop(&acceptor_::listen());
}

// 在连接建立的时候，默认的动作时打印双方的地址；
void TcpServer::defaultOnConnectionCallback(const TcpConnection *conn)
{
    LOG << conn->getLocalAddr().toIpPort() << "->" << conn->getPeerAddr().toIpPort() << std::endl;
}

void TcpServer::defaultOnMessageCallback(const TcpConnection *conn, Buffer* buffer)
{
    std::string msg(buffer->retrieveAllAsString());
    LOG << conn->getConnName() << " recv " << msg.size() << "bytes" << std::endl; 
}
