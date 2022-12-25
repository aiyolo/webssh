#include "../webssh/EventLoopThread.h"
#include "../webssh/EventLoop.h"
#include "../webssh/Callbacks.h"

void task(){
    printf("runinloop\n");
}

void task2()
{
    printf("queueinloop\n");
}

int main(){
    EventLoopThread *t = new EventLoopThread();
    EventLoop* loop = t->startLoop();
    loop->runInLoop(task);
    loop->queueInLoop(task2);
    while(1);
    return 0;
}