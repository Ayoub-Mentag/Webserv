#include <FormRequest.hpp>

// parse simple request

FormRequest::FormRequest(REQUEST_TYPE type) : Request(type) {}

FormRequest::FormRequest() {}

void	FormRequest::parseBody(std::string body) {
	this->body = body;
}

std::string	FormRequest::getBody() const
{
	return this->body;
}

void	FormRequest::setBody(std::string body) {
	this->body = body;
}
