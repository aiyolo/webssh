#include "Channel.h"
#include <sys/epoll.h>

Channel::Channel(int fd):fd_(fd), events_(0), revents_(0){}   

Channel::~Channel(){}

void Channel::enableReading(){
    events_ = EPOLLIN | EPOLLET;

}

void Channel::handleEvent(){
    
}
void Channel::setRevents(uint32_t revt)
{
    revents_ = revt;
}

int Channel::getFd(){
    return fd_;
}

int Channel::getEvents(){
    return events_;
}

int Channel::getRevents(){
    return revents_;
}

