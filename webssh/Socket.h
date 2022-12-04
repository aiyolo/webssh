#pragma once

class InetAddress;
class Socket {
public:
    Socket(int fd):sockfd_(fd){}
    Socket();
    void listen();
    void bind(InetAddress* addr);
    int accept(InetAddress* peeraddr);
    int getFd();
private:
  int sockfd_;
};