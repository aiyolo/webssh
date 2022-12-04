#include <iostream>
// #include "src/TcpServer.h"
#include "webssh/InetAddress.h"
#include "webssh/Socket.h"
#include "webssh/util.h"

using namespace std;

int main() {
  // TcpServer server();
  Socket servsock = Socket();
  InetAddress servaddr = InetAddress("127.0.0.1", 8888);
  servsock.bind(&servaddr);
  servsock.listen();
  InetAddress clntaddr = InetAddress();
  Socket clntsock = Socket(servsock.accept(&clntaddr));
  cout << clntsock.getFd();
}