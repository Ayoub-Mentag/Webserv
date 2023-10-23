#include <PostRequest.hpp>

// parse simple request

PostRequest::PostRequest() : parsingType(NONE){}

PostRequest::PostRequest(int parsingType) : parsingType(parsingType){}

void	PostRequest::parseBody(std::string body) {
	this->body = body;
}

const std::string&	PostRequest::getBody() const {
	return this->body;
}

void	PostRequest::setBody(std::string body) {
	this->body = body;
}

const int&	PostRequest::getParsingType() const {
	return this->parsingType;
}

void	PostRequest::setParsingType(int& parsingType) {
	this->parsingType = parsingType;
}