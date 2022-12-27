#pragma once
#include <map>
#include <string>
#include "Callbacks.h"

class HttpRequest
{
public:
	HttpRequest()
		: method_(Method::kInvalid)
		, version_(Version::kUnknown)
	{}
	~HttpRequest()
	{}
	void setVersion(Version version)
	{
		version_ = version;
	}
	Version getVersion() const
	{
		return version_;
	}
	bool setMethod(std::string method)
	{
		if (method == "GET") {
			method_ = Method::kGet;
		}
		else if (method == "POST") {
			method_ = Method::kPost;
		}
		else if (method == "HEAD") {
			method_ = Method::kHead;
		}
		return method_ != Method::kInvalid;
	}
	void setPath(std::string path)
	{
		path_ = path;
	}
	const std::string& getPath() const
	{	// must return const
		return path_;
	}

	void setQuery(std::string query)
	{
		query_ = query;
	}
	const std::string& getQeury() const
	{
		return query_;
	}
	void addHeader(std::string key, std::string value)
	{
		headersMap_[key] = value;
	}
	const std::string getHeader(const std::string& key) const
	{
		auto it = headersMap_.find(key);
		if (it != headersMap_.end()) {
			return it->second;
		}
		return "";	 // operator[]方法不能被const方法调用
	}
	const std::map<std::string, std::string>& getHeaders() const
	{
		return headersMap_;
	}

	// Get / http1.1
	// connnection: keepalive
	Method							   method_;
	Version							   version_;
	std::string						   path_;
	std::string						   query_;
	std::map<std::string, std::string> headersMap_;
};