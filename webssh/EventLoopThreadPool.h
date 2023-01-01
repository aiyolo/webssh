#pragma once
#include <string>
#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop*);
    ~EventLoopThreadPool();

    void start();
    void setThreadNum(int numThreads);
    EventLoop* getNextLoop();

    EventLoop*	baseloop_;
    bool started_;
    int next_;
	std::string				name_;
	int						numThreads_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};