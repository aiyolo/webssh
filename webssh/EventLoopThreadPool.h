#pragma once
#include <string>
#include <vector>

class EventLoop;
class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop*);
    ~EventLoopThreadPool();

    void start();
    void getNextLoop();

    EventLoop*	baseloop_;
	std::string				name_;
	int						numThreads_;
	std::vector<EventLoop*> loops_;
};