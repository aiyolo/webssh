#pragma once
#include "Buffer.h"
#include "Callbacks.h"
#include "HttpRequest.h"

class HttpContext
{
public:
	HttpContext()
		: state_(HttpRequestParseState::kExpectRequestLine){};

	bool parseRequest(Buffer* buf)
	{
		bool ok		 = true;
		bool hasMore = true;
		while (hasMore) {
            // 请求行
			if (state_ == HttpRequestParseState::kExpectRequestLine) {
				const char* crlf = buf->findCRLF();
				if (crlf) {
					ok = processRequestLine(buf->peek(), crlf);
					if (ok) {
						//   request_.setReceiveTime(receiveTime);
						buf->retrieveUntil(crlf + 2);
						state_ = HttpRequestParseState::kExpectHeaders;
					}
					else {
						hasMore = false;
					}
				}
				else {
					hasMore = false;
				}
			}
            // 请求头
			else if (state_ == HttpRequestParseState::kExpectHeaders) {
				const char* crlf = buf->findCRLF();
				if (crlf) {
					const char* colon = std::find(buf->peek(), crlf, ':');
					if (colon != crlf) {
                        std::string key = std::string(buf->peek(), colon);
                        std::string value = std::string(colon+1, crlf);
						// request_.addHeader(buf->peek(), colon, crlf);
                        request_.addHeader(key, value);
					}
					else {
						// empty line, end of header
						// FIXME:
						state_	= HttpRequestParseState::kGotAll;
						hasMore = false;
					}
					buf->retrieveUntil(crlf + 2);
				}
				else {
					hasMore = false;
				}
			}
            // 请求体
			else if (state_ == HttpRequestParseState::kExpectBody) {
				// FIXME:
			}
		}
		return ok;
	}

    // 解析请求行
	bool processRequestLine(const char* begin, const char* end)
	{
        // 请求行字符串格式："Get /index.html?question=quest HTTP1.1\r\n"
		bool		succeed = false;
		const char* start	= begin;
		const char* space	= std::find(start, end, ' '); // <algorithm> is needed
        // 首字符与第一个空格之间的字符串为method
        std::string method = std::string(start, space);
		if (space != end && request_.setMethod(method))
        {
			start = space + 1;
			space = std::find(start, end, ' ');
			if (space != end) {
                // 解析 path
				const char* question = std::find(start, space, '?');
				if (question != space) {
                    std::string path = std::string(start, question);
                    std::string query = std::string(question, space);
					request_.setPath(path);
					request_.setQuery(query);
				}
				else {
                    std::string path = std::string(start, space);
					request_.setPath(path);
				}
                // 解析version
				start	= space + 1;
				succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
				if (succeed) {
					if (*(end - 1) == '1') {
						request_.setVersion(Version::kHttp11);
					}
					else if (*(end - 1) == '0') {
						request_.setVersion(Version::kHttp10);
					}
					else {
						succeed = false;
					}
				}
			}
		}
		return succeed;
	}
	bool gotAll() const
	{
		return state_ == HttpRequestParseState::kGotAll;
	}
	void reset()
	{
		state_ = HttpRequestParseState::kExpectRequestLine;
		new (&request_) HttpRequest();
	}
	const HttpRequest& getRequest() const
	{
		return request_;
	}
	HttpRequestParseState state_;
	HttpRequest			  request_;
};