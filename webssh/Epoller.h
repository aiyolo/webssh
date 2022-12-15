#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <vector>
#include <map>

class Channel;
class Epoller{
public:
    Epoller();
    ~Epoller();
    void poll(int timeout, std::vector<Channel*> *activeChannels); 
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void update(int operation, Channel* channel);
    std::string opToString(int op);
    int epfd_;
    std::vector<struct epoll_event> events_;
    std::map<int,Channel*> channels_;
};