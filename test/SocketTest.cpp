#include <iostream>
#include "../webssh/InetAddress.h"
#include "../webssh/Socket.h"
#include "../webssh/util.h"
#include "../webssh/TcpServer.h"
#include "../webssh/Epoller.h"

using namespace std;
int main()
{
    Socket socket = Socket();
    InetAddress addr(8888);
    socket.bind(addr);
    socket.listen();
    InetAddress peerAddr;
    socket.accept(&peerAddr);
    cout << peerAddr.toIpPort();
}
