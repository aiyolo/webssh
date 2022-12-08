#include <iostream>
// #include "src/TcpServer.h"
#include "../webssh/InetAddress.h"
#include "../webssh/Socket.h"
#include "../webssh/util.h"
#include "../webssh/TcpServer.h"

using namespace std;

int main() {
  InetAddress listenAddr(8888, true);
  cout << listenAddr.toPort();
  // TcpServer server()
}