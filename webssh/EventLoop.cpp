#include "EventLoop.h"
#include "Callbacks.h"
#include "Channel.h"
#include "Epoller.h"
#include "Socket.h"
#include "util.h"
#include <iterator>
#include <mutex>
#include <sys/eventfd.h>   // eventfd
#include <sys/types.h>
#include <thread>
#include <assert.h>

EventLoop::EventLoop()
	: quit_(false)
	, callingPendingFunctors_(false)
	, wakeupFd_(createEventFd())
	, eventHandling_(false)
	, wakeupChannel_(new Channel(this, wakeupFd_))
	, currentActiveChannel_(nullptr)
	, poller_(new Epoller(this))
	, threadId_(std::this_thread::get_id())
{
	wakeupChannel_->setOnReadEventCallback(std::bind(&EventLoop::readWakeupFd, this));
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{}

void EventLoop::loop()
{
	// std::cout << "threadId_:" << threadId_ << "this threadId:" << std::this_thread::get_id() << std::endl;
	assertInLoopThread();
	// PrintFuncName pf("EventLoop::loop");
	quit_ = false;
	while (!quit_) {
		activeChannels_.clear();
		poller_->poll(kPollTimeMs, &activeChannels_);
		eventHandling_ = true;
		for (Channel* channel : activeChannels_)
		 { 
			currentActiveChannel_ = channel;
			currentActiveChannel_->handleEvent();
			// channel->handleEvent();
		}
		currentActiveChannel_ = nullptr;
		eventHandling_ = false;
		doPendingFunctors();
	}
}

void EventLoop::quit()
{
	quit_ = true;
	if(!isInLoopThread())
	{
		wakeup();
	}
}
void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
	assertInLoopThread();
	if(eventHandling_)
	{
		assert(currentActiveChannel_==channel || std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
	}
	poller_->removeChannel(channel);
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for (const auto& task : functors) { task(); }
	callingPendingFunctors_ = false;
}

void EventLoop::runInLoop(const Functor& cb)
{
	// std::cout << "runinloop:" << std::this_thread::get_id() << std::endl;
	if (isInLoopThread()) {
		// std::cout << "inloopthread threadId_:" << threadId_ << std::endl;
		cb();
	}
	else {
		queueInLoop(std::move(cb));
	}
}

void EventLoop::queueInLoop(const Functor& cb)
{
	// std::cout << "queueinloop threadId_:" << threadId_ << std::endl;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		pendingFunctors_.push_back(std::move(cb));
	}
	if (!isInLoopThread() || callingPendingFunctors_) {	  // note
		wakeup();
	}
}

void EventLoop::wakeup()
{
	// eventfd 每次需要输入8字节
	uint64_t one = 1;
	ssize_t	 n	 = Socket::write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG << "wakeup write error...\n";
	}
}

int EventLoop::createEventFd()
{
	int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evfd < 0) {
		LOG << "eventfd error...\n";
		abort();
	}
	return evfd;
}

void EventLoop::readWakeupFd()
{
	uint64_t one = 1;
	ssize_t	 n	 = Socket::read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG << "readWakeupFd error...\n";
	}
}

bool EventLoop::isInLoopThread()
{
	return threadId_ == std::this_thread::get_id();
}


void EventLoop::assertInLoopThread()
{
	if (!isInLoopThread()) {
		LOG << "not isInLoopThread...\n";
	}
}

bool EventLoop::hasChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	return poller_->hasChannel(channel);
}

void EventLoop::printActiveChannels() const
{
	for(const auto& channel: activeChannels_)
	{
		LOG << "{" << channel->reventsToString() << "} ";
	}
}