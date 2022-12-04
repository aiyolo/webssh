#include "InetAddress.h"
#include <cstring>

InetAddress::InetAddress(){
    bzero(&addr_, sizeof(addr_));
    addrlen_ = sizeof(addr_);
}

InetAddress::InetAddress(std::string ip, uint16_t port){
    memset(&addr_, 0, sizeof(addr_)); // <cstring>
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addrlen_ = sizeof(addr_);
}

sockaddr_in InetAddress::getAddr(){
    return addr_;
}