#pragma once
#include <cstdint>
#include <iostream>
#include <arpa/inet.h>

class InetAddress {
public:
  InetAddress(std::string ip, uint16_t port);
  InetAddress(uint16_t port = 0, bool loopbackOnly=false);
  sockaddr_in getAddr() const;
  std::string toIp() const;
  std::string toPort() const;
  std::string toIpPort() const;

public:
  struct sockaddr_in addr_;
  socklen_t addrlen_;
};
