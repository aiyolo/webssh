#pragma once
#include "Epoll.h"
#include <vector>
#include <memory> // unique_ptr

#define PollTimeMs 10000

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void doPendingFunctors(); // 处理其他事件

    bool quit_;
    std::vector<Channel*> activeChannels_;
    std::unique_ptr<Epoll> poller_;
};