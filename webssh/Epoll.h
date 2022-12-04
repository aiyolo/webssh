#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll{
public:
    Epoll();
    ~Epoll();
    int epfd_;
    struct epoll_event* events_;
    void poll(int timeout, std::vector<Channel*> *activeChannels); 
};