#pragma once

#include "Callbacks.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class EventLoop;

class EventLoopThread
{
public: 
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();
    void threadFunc();

    EventLoop* loop_;
    bool exit_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};