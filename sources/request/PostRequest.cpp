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

const std::vector<t_data>&	PostRequest::getData() const {
	return (this->data);
}

void	PostRequest::setDataByValues(std::map<std::string, std::string> head, std::string content) {
	t_data d;

	d.head = head;
	d.content = content;
	this->data.push_back(d);
}
t_data	&PostRequest::getDataByIndex(size_t index) {
	return this->data[index];
}

void	PostRequest::setContent(std::string content, size_t index) {
	this->getDataByIndex(index).content = content;
}