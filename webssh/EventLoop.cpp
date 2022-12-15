#include "EventLoop.h"
#include <iterator>
#include "Channel.h"
#include "Epoller.h"
#include "util.h"

EventLoop::EventLoop():quit_(false), poller_(new Epoller()){

}

EventLoop::~EventLoop(){

}

void EventLoop::loop(){
    PrintFuncName pf("EventLoop::loop");
    while(!quit_){
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        for(Channel* channel: activeChannels_){
            channel->handleEvent();
        }
        doPendingFunctors();
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    PrintFuncName pf("EventLoop::updateChannel\n");
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}
void EventLoop::doPendingFunctors(){
    
}