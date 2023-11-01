#include <PostRequest.hpp>

// parse simple request

PostRequest::PostRequest() {}

PostRequest::PostRequest(std::map<std::string, std::string> head) : Request(head){}

const std::string&	PostRequest::getBody() const {
	return this->body;
}

void	PostRequest::setBody(std::string body) {
	this->body = body;
}

