#include "Epoller.h"
#include "Callbacks.h"
#include "Channel.h"
#include "EventLoop.h"
#include "util.h"
#include <assert.h>
#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#define kInitEventListSize 16
#define kNew -1
#define kAdded 1
#define kDeleted 2

Epoller::Epoller(EventLoop* loop)
	: epfd_(epoll_create(EPOLL_CLOEXEC))
	, events_(kInitEventListSize)
	, ownerLoop_(loop)
{
	if (epfd_ < 0) {
		LOG << "epoll_creat error...\n";
	}
}

Epoller::~Epoller()
{
	// if (epfd_ != -1) {
	// 	::close(epfd_);
	// 	epfd_ = -1;
	// }
	::close(epfd_);
}

void Epoller::updateChannel(Channel* channel)
{
	// PrintFuncName pf("Epoller::updatechannel");
	ownerLoop_->assertInLoopThread();
	const int index = channel->getIndex();
	// LOG << "fd = " << channel->getFd() << " events = " << channel->getEvents() << " index = " << index << std::endl;
	int fd = channel->getFd();
	if (index == kNew || index == kDeleted) {
		if (index == kNew) {
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		// 之前已经添加到map但是没有活跃事件，被标记乘kDeleted的channel，重新活跃起来了，需要重新添加到epfd_实例上
		else   // index == kDeleted
		{
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}
		channel->setIndex(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else   // index == kAdded
	{
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		// 如果channel上没有事件，在epfd上移除相应的fd
		assert(index == kAdded);
		if (channel->isNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->setIndex(kDeleted);
		}
		else {
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void Epoller::removeChannel(Channel* channel)
{
	ownerLoop_->assertInLoopThread();
	int fd = channel->getFd();
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->isNoneEvent());
	int index = channel->getIndex();
	assert(index == kAdded || index == kDeleted);
	size_t n = channels_.erase(fd);
	assert(n == 1);
	if (index == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->setIndex(kNew);
}

void Epoller::update(int operation, Channel* channel)
{

	struct epoll_event event;
	memset(&event, 0, sizeof event);
	event.events   = channel->getEvents();
	event.data.ptr = channel;
	int fd		   = channel->getFd();
	int ret		   = ::epoll_ctl(epfd_, operation, fd, &event);
	if (ret < 0) {
		LOG << "Epoller::updatedEvents error...";
	}
}

void Epoller::poll(int timeout, std::vector<Channel*>* activeChannels)
{
	// PrintFuncName pf("Epoller::poll");
	int nfds = ::epoll_wait(epfd_, &*events_.begin(), events_.size(), timeout);
	int savedErrno = errno;
	if (nfds > 0) {
		assert(static_cast<size_t>(nfds) <=events_.size());
		for (int i = 0; i < nfds; i++) {
			Channel* ch = static_cast<Channel*>(events_[i].data.ptr);
			ch->setRevents(events_[i].events);
			activeChannels->emplace_back(ch);
		}
		if(static_cast<size_t>(nfds) == events_.size()){
			events_.resize(events_.size()*2);
		}
	}
	else if(nfds==0)
	{
		LOG << "nothing happended...\n";
	}
	else {
		if(savedErrno!=EINTR)
		{
			errno = savedErrno;
			LOG << "epoll_wait error...\n" << nfds << errno;
		}
	}
}


std::string Epoller::opToString(int op)
{
	switch (op) {
	case EPOLL_CTL_ADD: return "ADD";
	case EPOLL_CTL_DEL: return "DEL";
	case EPOLL_CTL_MOD: return "MOD";
	default: return "Unknown Operation";
	}
}

bool Epoller::hasChannel(Channel* channel) const
{
	assertInLoopThread();
	auto it = channels_.find(channel->getFd());
	return it != channels_.end() && it->second == channel;
}

void Epoller::assertInLoopThread() const
{
	ownerLoop_->assertInLoopThread();
}