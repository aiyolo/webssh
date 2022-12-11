#pragma once
#include "Epoller.h"
#include <vector>
#include <memory> // unique_ptr

#define kPollTimeMs 10000

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void doPendingFunctors(); // 处理其他事件
    void updateChannel(Channel *channel);

    bool quit_;
    std::vector<Channel*> activeChannels_;
    std::unique_ptr<Epoller> poller_;
};