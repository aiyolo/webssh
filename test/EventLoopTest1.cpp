#include "../webssh/EventLoopThread.h"
#include "../webssh/EventLoop.h"
#include "../webssh/Callbacks.h"
#include <thread>

EventLoop *gloop;
void task1()
{
    gloop->loop();
}

int main(){
    EventLoop loop;
    gloop = &loop;
    std::thread t1(task1);
    loop.loop();
    return 0;
}