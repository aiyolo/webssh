#include "EventLoop.h"
#include <iterator>
#include "Channel.h"


void EventLoop::loop(){
    while(!quit_){
        activeChannels_.clear();
        poller_->poll(PollTimeMs, &activeChannels_);
        for(Channel* channel: activeChannels_){
            channel->handleEvent();
        }
        doPendingFunctors();
    }
}

void EventLoop::doPendingFunctors(){
    
}