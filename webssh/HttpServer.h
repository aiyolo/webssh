#pragma once

#include "Callbacks.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpServer.h"
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)> ;

    HttpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name);
    ~HttpServer();
    void setHttpCallback(const HttpCallback& cb);
    void start();
    static void defaultHttpCallback(const HttpRequest& req, HttpResponse* resp);
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buff);
    void onRequest(const TcpConnectionPtr& conn, const HttpRequest&);

    TcpServer server_;
    HttpCallback httpCallback_;
};