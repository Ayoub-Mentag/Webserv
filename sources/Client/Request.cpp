#include <Request.hpp>


Request::Request() {}

Request::Request(std::map<std::string, std::string> head) {
	this->head = head;
}

Request::~Request() {}

void	Request::setHead( std::map<std::string, std::string>  head) {
	this->head = head;
}


std::map<std::string, std::string>&	Request::getHead() {
	return this->head;
}

void	Request::addToHead(std::string key, std::string value) {
	this->head[key] = value;
}


std::string&	Request::getValueByKey(std::string key) {
	return (this->head[key]);
}