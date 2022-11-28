#pragma once

class HttpRequest;
class HttpResponse;

class HttpServer{
public:
    typedef std::function<void(const HttpRequest&, HttpResponse*)> HttpCallback;
    HttpServer(EventLoop* loop);
    EventLoop* getloop() const { return server_.getLoop();}
    void setHttpCallback(const HttpCallback& cb){
        httpCallback_ = cb;
    }
    void setThreadNum(int numThreads){
        server_.setThreadNum(numThreads);
    }
    void start();
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp receiveTime);
    void onRequest(const TcpConnectioinPtr&, cnst HttpRequest&);
    TcpServer server_;
    HttpCallback httpCallBack_;

};