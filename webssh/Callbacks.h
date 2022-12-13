#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <vector>

class Buffer;
class TcpConnection;
class InetAddress;
class Channel;


template <typename T> T *getPointer(const std::shared_ptr<T> &ptr) {
  return ptr.get();
}

template <typename T> T *getPointer(const std::unique_ptr<T> &ptr) {
  return ptr.get();
}

// using std::placeholders::_1;
// using std::placeholders::_2;

using ChannelList = std::vector<Channel>;
// connection 要传给不同的functor，使用shared_ptr
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

using NewConnectionCallback = std::function<void(int, const InetAddress &)>;

using OnConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using OnMessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *)>;
using OnCloseCallback = std::function<void(const TcpConnectionPtr&)>;
using OnWriteCompleCallback = std::function<void(const TcpConnectionPtr&)>;

using OnReadEventCallback = std::function<void()>;
using OnWriteEventCallback = std::function<void()>;
using OnCloseEventCallback = std::function<void()>;
using OnErrorEventCallback = std::function<void()>;

void defaultOnConnectionCallback(const TcpConnectionPtr&conn);

void defaultOnMessageCallback(const TcpConnectionPtr& , Buffer *);