#include <BoundaryRequest.hpp>
#include <Utils.hpp>

void	parseHead(std::map<std::string, std::string>& headMap, size_t i, std::vector<std::string> lines);

std::vector<std::map<std::string, std::string> >&	BoundaryRequest::getBody() {
	return (this->body);
}

BoundaryRequest::BoundaryRequest(REQUEST_TYPE type) : Request(type) {}

BoundaryRequest::BoundaryRequest() {}

void	BoundaryRequest::parseBody(std::string body) {
	std::string 	boundary;
	size_t			index;
	std::string 	tmpString;
	std::string 	header;

	boundary	=	head["Boundary"];
	index		=	body.find(boundary + "\r\n");
	if (index == std::string::npos)
		throw std::runtime_error("No boundaries found");
	while (1) {
		std::map<std::string, std::string> boundaryMap;
		index += boundary.size() + 2;
		if (index >= body.size())
			break ;
		tmpString = &body[index];

		header = tmpString.substr(0, tmpString.find("\r\n\r\n"));
		parseHead(boundaryMap, 0, splitLine(header, "\r\n"));
		index += header.size() + 4;
		if (index >= body.size())
			break ;
		tmpString = &body[index];
		boundaryMap["PostEntity"] = tmpString.substr(0, tmpString.find(boundary + "\r\n"));
		index += boundaryMap["PostEntity"].size() + 2;
		this->body.push_back(boundaryMap);
	}
}

void	BoundaryRequest::setBoundary(std::string boundary) {
	this->boundary = boundary;
}

void	BoundaryRequest::setBody(std::vector<std::map<std::string, std::string> >  body) {
	this->body = body;
}

