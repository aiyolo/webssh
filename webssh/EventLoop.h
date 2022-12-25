#pragma once
#include "Callbacks.h"
#include "Channel.h"
#include "Epoller.h"
#include <vector>
#include <memory> // unique_ptr
#include <functional>
#include <thread>
#include <mutex>

#define kPollTimeMs 10000

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void doPendingFunctors(); // 处理其他事件
    void updateChannel(Channel *channel);
    void removeChannel(Channel* channel);
    void runInLoop(const Functor& cb);
    bool isInLoopThread();
    void queueInLoop(const Functor& cb);
    void wakeup();
    int createEventFd();
    void readWakeupFd();

    bool quit_;
    bool callingPendingFunctors_;
    int wakeupFd_;
    Channel* wakeupChannel_;
    Channel* currentActiveChannel_;
    std::mutex mutex_;
    std::unique_ptr<Epoller> poller_;
    std::thread::id threadId_;
    std::vector<Channel*> activeChannels_;
    std::vector<Functor> pendingFunctors_;
};