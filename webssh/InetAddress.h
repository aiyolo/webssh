#pragma once
#include <iostream>
#include <arpa/inet.h>

class InetAddress {
public:
  InetAddress();
  InetAddress(std::string ip, uint16_t port);
  sockaddr_in getAddr();

public:
  struct sockaddr_in addr_;
  socklen_t addrlen_;
};
