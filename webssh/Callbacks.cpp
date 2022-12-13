#include "Callbacks.h"
#include "util.h"
#include "TcpConnection.h"

// 在连接建立的时候，默认的动作时打印双方的地址；
void defaultOnConnectionCallback(const TcpConnectionPtr &conn)
{
    LOG << conn->getLocalAddr().toIpPort() << "->" << conn->getPeerAddr().toIpPort() << std::endl;
}

void defaultOnMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer)
{
    std::string msg(buffer->retrieveAllAsString());
    LOG << conn->getConnName() << " recv " << msg << std::endl; 
    conn->send(msg);
}