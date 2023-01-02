#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Logger.h"
#include <mutex>
#include "util.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop)
	: baseloop_(baseloop)
	, started_(false)
	, next_(0)
	, numThreads_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool(){}

void EventLoopThreadPool::start()
{
    baseloop_->assertInLoopThread();
    started_ =  true;
    for(int i=0; i<numThreads_; i++)
    {
        std::unique_ptr<EventLoopThread> evth(new EventLoopThread());
        loops_.push_back(evth->startLoop());
        threads_.push_back(std::move(evth));

        // EventLoopThread* t = new EventLoopThread();
        // threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        // loops_.push_back(t->startLoop());
    }

}

void EventLoopThreadPool::setThreadNum(int numThreads)
{
    numThreads_ = numThreads;
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseloop_->assertInLoopThread();
    EventLoop* loop = baseloop_;
    if(!loops_.empty()){
        // LOG << "next_ loop is " << next_ << std::endl;
        loop = loops_[next_];
        next_ = (next_+1)%numThreads_;
    }
    return loop;
}