#pragma once
#include "Buffer.h"
#include <cstdio>
#include <map>
#include <string>
#include "Callbacks.h"

class HttpResponse
{
public:
	HttpResponse(bool close)
		: statusCode_(HttpStatusCode::kUnknown)
		, closeConnection_(close)
	{}
	void setStatusCode(HttpStatusCode code)
	{
		statusCode_ = code;
	}
	void setStatusMessage(const std::string& message)
	{
		statusMessage_ = message;
	}
    void setCloseConnection(bool on)
    {
        closeConnection_ = on;
    }
    bool closeConnection() const 
    {
        return closeConnection_;
    }
	void setContentType(const std::string& contentType)
	{
		addHeader("Content-Type", contentType);
	}
	void addHeader(const std::string& key, const std::string& value)
	{
		headersMap_[key] = value;
	}
	void setBody(const std::string& body)
	{
		body_ = body;
	}
	void appendToBuffer(Buffer* outputBuffer) const
	{
		char buf[32];
		std::snprintf(buf, sizeof buf, "HTTP/1.1 %d ", static_cast<int>(statusCode_));
		outputBuffer->append(buf);
		outputBuffer->append(statusMessage_);
		outputBuffer->append("\r\n");

		if (closeConnection_) {
			outputBuffer->append("Connection: close\r\n");
		}
		else {
			snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
			outputBuffer->append(buf);
			outputBuffer->append("Connection: Keep-Alive\r\n");
		}

		for (const auto& header : headersMap_) {
			outputBuffer->append(header.first);
			outputBuffer->append(": ");
			outputBuffer->append(header.second);
			outputBuffer->append("\r\n");
		}

		outputBuffer->append("\r\n");
		outputBuffer->append(body_);
	}
	// http1.1 200 OK
	// connection: keepalive
	std::map<std::string, std::string> headersMap_;
	Version							   version_;
	HttpStatusCode					   statusCode_;
	std::string						   statusMessage_;
	bool							   closeConnection_;
	std::string						   body_;
};