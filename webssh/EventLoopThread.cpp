#include "EventLoopThread.h"
#include "EventLoop.h"
#include <mutex>

EventLoopThread::EventLoopThread()
	: loop_(nullptr)
	, exit_(false)
	, thread_(std::bind(&EventLoopThread::threadFunc, this))
	// , callback_(cb)
{}

EventLoopThread::~EventLoopThread()
{
    exit_ = true;
    if(loop_){
        loop_->quit();
        thread_.join();
    }
}

// 把子线程的loop指针回传到控制线程
EventLoop* EventLoopThread::startLoop(){
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_==nullptr){
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

// 在子线程中 loop 循环转动
void EventLoopThread::threadFunc(){
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop.loop();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}