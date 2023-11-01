#include <PostRequest.hpp>

// parse simple request

PostRequest::PostRequest() {}

PostRequest::PostRequest(std::map<std::string, std::string> head, std::string body) : Request(head), body(body){}

const std::string&	PostRequest::getBody() const {
	return this->body;
}

void	PostRequest::setBody(std::string body) {
	this->body = body;
}

