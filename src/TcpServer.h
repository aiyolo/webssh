#pragma once
class EventLoop;
class EventLoopThreadPool;

class TcpServer{
public:
    typedef std::function<void(EventLoop*)> threadInitCallback;

    TcpServer(
        EventLoop* loop
    )
    ~TcpServer();
    EventLoop* getLoop const {return loop_;}
    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb){}
    std::shared_ptr<EventLoopThreadPool> threadPool(){
        return threadPool_;
    }
    void start();
    void setConnectionCallback(const ConnectionCallback* cb){
        connectionCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){
        writeComplereCallback_ = cb;
    }
private:
    EvetLoop* loop_;
    
};