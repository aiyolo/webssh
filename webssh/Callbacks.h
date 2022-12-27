#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Buffer;
class TcpConnection;
class InetAddress;
class Channel;
class EventLoop;

enum class ConnState
{
	kDisconnected,
	kConnecting,
	kConnected,
	kDisconnecting
};

enum class HttpStatusCode
{
	kUnknown,
	k200 = 200,	  // OK
	k301 = 301,	  // moved permanently
	k400 = 400,	  // bad request
	k403 = 403,	  // forbidden
	k404 = 404	  // not found

};

enum class HttpRequestParseState
{
	kExpectRequestLine,
	kExpectHeaders,
	kExpectBody,
	kGotAll
};

enum class Method
{
	kInvalid,
	kGet,
	kPost,
	kHead,
};

enum class Version
{
	kUnknown,
	kHttp10,
	kHttp11
};

template<typename T> T* getPointer(const std::shared_ptr<T>& ptr)
{
	return ptr.get();
}

template<typename T> T* getPointer(const std::unique_ptr<T>& ptr)
{
	return ptr.get();
}

// using std::placeholders::_1;
// using std::placeholders::_2;

using ChannelList = std::vector<Channel>;
// connection 要传给不同的functor，使用shared_ptr
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionMap	   = std::map<std::string, TcpConnectionPtr>;

using NewConnectionCallback = std::function<void(int, const InetAddress&)>;

using OnConnectionCallback	= std::function<void(const TcpConnectionPtr&)>;
using OnMessageCallback		= std::function<void(const TcpConnectionPtr&, Buffer*)>;
using OnCloseCallback		= std::function<void(const TcpConnectionPtr&)>;
using OnWriteCompleCallback = std::function<void(const TcpConnectionPtr&)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;

using OnReadEventCallback  = std::function<void()>;
using OnWriteEventCallback = std::function<void()>;
using OnCloseEventCallback = std::function<void()>;
using OnErrorEventCallback = std::function<void()>;

using ThreadInitCallback = std::function<void(EventLoop*)>;
using Functor			 = std::function<void()>;

void defaultOnConnectionCallback(const TcpConnectionPtr& conn);

void defaultOnMessageCallback(const TcpConnectionPtr&, Buffer*);