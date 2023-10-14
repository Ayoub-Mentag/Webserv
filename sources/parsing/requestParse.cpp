#include <parsingHeader.hpp>
/** @brief
	This parsing based on these pillars
	1- HEADER
		/r/n/r/n
		BODY
	2 - The first line contains the METHOD PATH HTTPVERSION
	and the second one containg the HOST PORT
	and the rest would look like Key: Value
*/

// TODO: check if there are other types cuz it not handled yet
// like "Content-Type: application/json"
// add vector of QueryString
// the request should be corrected before using it

enum BODY_TYPE{
	SIMPLE,
	BOUNDARY,
	CHUNKED,
	JSON,
	NONE
};


static std::vector<std::string>	splitLine(std::string line, std::string delimiter) {
	std::vector<std::string>	result;
	std::string					tmpLine;
	size_t						end;

	end = 0;
	while (1) {
		end = line.find(delimiter);
		if (end == std::string::npos)
			end = line.length();
		tmpLine = line.substr(0, end);
		result.push_back(tmpLine);
        if (end + delimiter.size() > line.size())
            break ;
        line = &line[end + delimiter.size()];
	}
	return (result);
}

static bool	assignBoundary(std::map<std::string, std::string>& headMap) {
	/*
		std::string type = request.head["Content-Type"];
		size_t boundaryIndex;

		if (type.find("multipart/form-data") == std::string::npos)
			return ;
		request.head["Content-Type"] = "multipart/form-data";
		boundaryIndex = type.find("boundary=");
		if (boundaryIndex == std::string::npos)
			return ;

		std::string boundary = &type[boundaryIndex];
		boundary = boundary.erase(0, 9);// 9 is the "boundary=" length
		if (!boundary.empty() && (boundary[0] == '\"')) {
			//remove the double quotes
			boundary.erase(0, 1);
			boundary.erase(boundary.length() - 1, 1);
		}
		request.head["Boundary"] = boundary;
	*/

	std::string type = headMap["Content-Type"];
	size_t boundaryIndex = type.find("boundary");

	if (boundaryIndex == std::string::npos)
		return false;
	headMap["Content-Type"] = type.substr(0, boundaryIndex);

	std::string boundary = &type[boundaryIndex];

	boundary = boundary.erase(0, 9);// 9 is the "boundary=" length
	if (!boundary.empty() && (boundary[0] == '\"')) {
		//remove the double quotes
		boundary.erase(0, 1);
		boundary.erase(boundary.length() - 1, 1);
	}
	headMap["Boundary"] = boundary;
	return (true);
}


static void	parseTwoFirstLines(std::map<std::string, std::string> &headMap, std::vector<std::string> lines) {
	std::string line;
	std::vector<std::string> firstLine;
	std::vector<std::string> secondLine;
	size_t		size;

	if (lines.size() > 2) {
		std::istringstream iss1(lines[0]);
		while (std::getline(iss1, line, ' ')) {
			firstLine.push_back(line);
		}
		if (firstLine.size() == 3) {
			headMap["Method"]  = firstLine[0];
			headMap["Path"]    = firstLine[1];
			headMap["HttpVer"] = firstLine[2];
		}
		std::istringstream iss2(lines[1]);
		while (std::getline(iss2, line, ':')) {
			secondLine.push_back(line);
		}
		size = secondLine.size();
		if (size == 2)
			headMap["Host"] = secondLine[1];
		if (size == 3)
			headMap["Port"] = secondLine[2];
	}
}

// TODO: remove the (size_t i) param
static void	parseHead(std::map<std::string, std::string>& headMap, size_t i, std::vector<std::string> lines) {
	std::string 						key;
	std::string 						value;

	/*
	if (typeOfHeader == HEADER_REQUEST) {
		if (lines.size() > 2) {
			std::istringstream iss1(lines[0]);
			while (std::getline(iss1, line, ' ')) {
				firstLine.push_back(line);
			}
			if (firstLine.size() == 3) {
				headMap["Method"]  = firstLine[0];
				headMap["Path"]    = firstLine[1];
				headMap["HttpVer"] = firstLine[2];
			}

			std::istringstream iss2(lines[1]);
			while (std::getline(iss2, line, ':')) {
				secondLine.push_back(line);
			}
			size = secondLine.size();
			if (size == 2)
				headMap["Host"] = secondLine[1];
			if (size == 3)
				headMap["Port"] = secondLine[2];
		}
		i += 2;
	}

	*/
	for (; i < lines.size(); i++) {
		std::istringstream iss3(lines[i]);
		std::getline(iss3, key, ':');
		std::getline(iss3, value, ':');
		if (value[0] == ' ')
			value.erase(0, 1);
		headMap[key] = value;
	}
}

/** @brief this function specify the body type ,and init the request map 
 * with appropriate elements [if we have a boundary it would be assigned ..]
 * if the type is none then we don't need to parse the request
*/
static BODY_TYPE	getTypeOfRequestBody(std::map<std::string, std::string>& headMap) {
	std::string type;

	if (headMap["Method"] == "POST") {
		if (assignBoundary(headMap))
			return BOUNDARY;
		type = headMap["Transfer-Encoding"];
		if (type == "Chunked")
			return (CHUNKED);
		type = headMap["Content-Type"];
		if (type == "application/x-www-form-urlencoded")
			return (SIMPLE);
		if (type == "application/json")
			return (JSON);
	}
	return (NONE);
}

std::string	checkRequest(std::string buffer) {
	std::string correctRequest;
}


Request	*generateRequest(int type) {
	switch (type) {
		case BOUNDARY :
			return new BoundaryRequest();
		case SIMPLE :
			return NULL;
		case CHUNKED : 
			return NULL;
		case JSON : 
			return NULL;
		default :
			return NULL;
	}
}

Request::Request() {}

BoundaryRequest::BoundaryRequest() {}

void	BoundaryRequest::setBoundary(std::string boundary) {
	this->boundary = boundary;
}

void	BoundaryRequest::parseBody(std::string body) {
	std::string 	boundary;
	size_t			index;
	std::string 	tmpString;
	std::string 	header;

	boundary	=	head["Boundary"];
	index		=	body.find(boundary + "\r\n");
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
		this->body.push_back(boundaryMap);
	}
	printRequest();

}

/** @example will start from the line that contains the query params
*/
void	SimpleRequest::parseSimpleBody(std::string body) {
	queryString = body;
	// std::cout << request.request["Query-String"] << std::endl;
}

// static void	parseBoundaryBody(t_request& request, std::string body)
// void	BoundaryRequest::parseBody(std::string body) {
// 	std::string 	boundary;
// 	size_t			index;
// 	std::string		header;
// 	std::string		tmpString;
// 	bounderyBody	b;

// 	boundary	=	request.head["Boundary"];
// 	index		=	body.find(boundary + "\r\n");
// 	while (1) {
// 		index += boundary.size() + 2;
// 		if (index >= body.size())
// 			break ;
// 		tmpString = &body[index];
// 		header = tmpString.substr(0, tmpString.find("\r\n\r\n"));
// 		// parseHeader(map, index, lines(\r\n));
// 		parseHead(b.header, 0, splitLine(header, ": "));
// 		index += header.size() + 4;
// 		if (index >= body.size())
// 			break ;
// 		tmpString = &body[index];
// 		b.postEntity = tmpString.substr(0, tmpString.find(boundary + "\r\n"));
// 		addABoundary(b);
// 	}
// }

void	BoundaryRequest::printRequest() {
	size_t i = 0;
	std::vector<std::map<std::string, std::string> > m = body;
}

void	Request::setHead( std::map<std::string, std::string>  head) {
	this->head = head;
}

void	BoundaryRequest::setBody(std::vector<std::map<std::string, std::string> >  body) {
	this->body = body;
}

/** @brief this parse does not check if the request is bad formated
 * //TODO: look for when a request is bad;
*/
Request 	*requestParse(std::string buffer) {
	size_t								index;
	std::string							head;
	std::string							body;
	Request								*request;
	std::map<std::string, std::string >	headMap;
	std::vector<std::string>			lines;

	index = buffer.find("\r\n\r\n");
	if (index == std::string::npos)
		throw std::runtime_error("Bad request");

	head 			= buffer.substr(0, index);
	lines 			= splitLine(head, "\r\n");

	parseTwoFirstLines(headMap, lines);
	parseHead(headMap, 2, lines);

	request = generateRequest(getTypeOfRequestBody(headMap));
	request->setHead(headMap);
	if (index + 4 < buffer.length())
	{
		body 	= buffer.substr(index + 4, buffer.length());
		request->parseBody(body);
	}

	return (request);
}