#include <iostream>
#include "../webssh/InetAddress.h"
#include "../webssh/Socket.h"
#include "../webssh/util.h"
#include "../webssh/TcpServer.h"
#include "../webssh/Epoller.h"

using namespace std;
int main()
{
    EventLoop loop;
    InetAddress listenAddr(8888, true);
    TcpServer server(&loop, listenAddr);
    server.start();
    loop.loop();
}
