#include <PostRequest.hpp>

// parse simple request

PostRequest::PostRequest() {}

const std::string&	PostRequest::getBody() const {
	return this->body;
}

void	PostRequest::setBody(std::string body) {
	this->body = body;
}

