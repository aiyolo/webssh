#include "../webssh/EventLoopThread.h"
#include "../webssh/EventLoop.h"
#include "../webssh/Callbacks.h"
#include <thread>

void task(){
    std::cout << "task threadid" << std::this_thread::get_id() << std::endl;
    printf("runinloop\n");
}

void task2()
{
    std::cout << "task2 threadid" << std::this_thread::get_id() << std::endl;
    printf("queueinloop\n");
}

int main(){
    EventLoopThread *t = new EventLoopThread();
    std::cout << "threadId_:" << std::this_thread::get_id() << std::endl;
    EventLoop* loop = t->startLoop();
    loop->runInLoop(task);
    loop->queueInLoop(task2);
    while(1);
    return 0;
}