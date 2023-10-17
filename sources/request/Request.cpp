#include <Request.hpp>

Request::Request(REQUEST_TYPE type) : typeOfRequest(type) {}

Request::Request() {}

Request::~Request() {}

void	Request::setHead( std::map<std::string, std::string>  head) {
	this->head = head;
}

void	Request::setTypeOfRequest(REQUEST_TYPE type) {
	this->typeOfRequest = type;
}

std::map<std::string, std::string>&	Request::getHead() {
	return this->head;
}

REQUEST_TYPE	Request::getTypeOfRequest() const {
	return (this->typeOfRequest);
}
