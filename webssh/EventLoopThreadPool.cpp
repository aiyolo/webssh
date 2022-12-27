#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop)
	: baseloop_(baseloop)
	, started_(false)
	, next_(0)
	, numThreads_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool(){}

void EventLoopThreadPool::start()
{
    started_ =  true;
    for(int i=0; i<numThreads_; i++)
    {
        std::unique_ptr<EventLoopThread> evth(new EventLoopThread());
        threads_.push_back(std::move(evth));

        loops_.push_back(evth->startLoop());
    }

}


EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseloop_;
    if(!loops_.empty() && numThreads_!=0){
        loop = loops_[next_];
        next_ = (next_+1)%numThreads_;
    }
    return loop;
}