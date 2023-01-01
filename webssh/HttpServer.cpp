#include "HttpServer.h"
#include "InetAddress.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpConnection.h"
#include "HttpContext.h"

void HttpServer::defaultHttpCallback(const HttpRequest &req, HttpResponse *resp)
{
    
}

HttpServer::HttpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name)
	: server_(loop, name, listenAddr)
    , httpCallback_(defaultHttpCallback)
{
    server_.setOnConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setOnMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer(){}

void HttpServer::setThreadNum(int numThreads)
{
    server_.setThreadNum(numThreads);
}

void HttpServer::start()
{
    server_.start();
}

void HttpServer::setHttpCallback(const HttpCallback &cb)
{
    httpCallback_ = cb;
}

void HttpServer::onConnection(const TcpConnectionPtr &conn)
{
    conn->setContext(HttpContext());
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, Buffer *buff)
{
    // context 用来解析请求buff数据,得到 request 结构体
    HttpContext context = conn->getContext();
    if(!context.parseRequest(buff))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if(context.gotAll())
    {
        onRequest(conn, context.getRequest());
        context.reset();
    }
}

// 根据 conn 收到的 request， 生成response
// 把 response 添加到 buffer，发送出去
// 根据要求 决定是否关闭连接
void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &req)
{
    std::string connection = req.getHeader("Connection");
    bool close = connection =="close" || (req.getVersion() == Version::kHttp10 && connection != "Keep-Alive");
    HttpResponse resp(close);
    httpCallback_(req, &resp);
    Buffer buffer;
    resp.appendToBuffer(&buffer);
    conn->send(&buffer);
    if(resp.closeConnection())
    {
        conn->shutdown();
    }
}