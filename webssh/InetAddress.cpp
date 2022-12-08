#include "InetAddress.h"
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>


InetAddress::InetAddress(std::string ip, uint16_t port)
{
    memset(&addr_, 0, sizeof(addr_)); // <cstring>
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addrlen_ = sizeof(addr_);
}

InetAddress::InetAddress(uint16_t port, bool loopbackOnly){
    memset(&addr_, 0, sizeof(addr_)); // <cstring>
    in_addr_t ip = loopbackOnly? INADDR_LOOPBACK:INADDR_ANY;
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(ip);
    addrlen_ = sizeof(addr_);
}

sockaddr_in InetAddress::getAddr() const{
    return addr_;
}

std::string InetAddress::toIp() const{
    char buf[64];
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, static_cast<socklen_t>(sizeof(buf)));
    return buf;
}

std::string InetAddress::toPort() const{
    char buf[64];
    uint16_t port = ::ntohs(addr_.sin_port);
    snprintf(buf, 64, "%d", port);
    return buf;
}