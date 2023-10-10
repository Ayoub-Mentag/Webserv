#include <serverHeader.hpp>
		
Response::Response() {}
Response::~Response() {}

const std::string&	Response::getHeader() {
	return (this->header);
}

const std::string&	Response::getBody() {
	return (this->body);
}

const std::string& 	Response::getStatusCode(int status) {
	return (this->statusCode[status]);
}

const std::string&	Response::getHttpVersion() {
	return (this->httpVersion);
}

const std::string&	Response::getContentType() {
	return (this->contentType);
}

std::string	Response::getBodylength() {
	return (to_string(this->body.length()));
}

void	Response::setHeader(int status) {
	header = getHttpVersion();
	header += getStatusCode(status) + "\r\n";
	header += "Content-type: " + getContentType();
	header += " Content-length: " + getBodylength();
	header += " \r\n\r\n";
}

void	Response::setBody(const std::string& body) {
	this->body = body;
}


void	Response::setHttpVersion(const std::string& httpVersion) {
	this->httpVersion = httpVersion;
}

void	Response::setContentType(const std::string& contentType) {
	this->contentType = contentType;
}

void	Response::setResponse() {
	this->response = getHeader() + getBody();
}

const std::string&	Response::getResponse() {
	setResponse();
	return (response);
}


void	Response::setStatusCode() {
	statusCode[200] = " 200 OK";
	statusCode[301] = " 301 Moved Permanently";
	statusCode[403] = " 403 Forbidden";
	statusCode[404] = " 404 Not Found";
	statusCode[405] = " 405 Method Not Allowed";
	statusCode[501] = " 501 Not Implemented";
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;
	// statusCode[] = ;

}
