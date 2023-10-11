#include <parsingHeader.hpp>
// TODO: check if there are other types cuz it not handled yet
// like "Content-Type: application/json"
// add vector of QueryString

enum BODY_TYPE{
	SIMPLE,
	BOUNDARY,
	CHUNKED,
	NONE
};

static void	assignBoundary(t_request &request) {
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

	std::string type = request.head["Content-Type"];
	size_t boundaryIndex = type.find("boundary");

	if (boundaryIndex == std::string::npos)
		return ;
	request.head["Content-Type"] = type.substr(0, boundaryIndex);

	std::string boundary = &type[boundaryIndex];

	boundary = boundary.erase(0, 9);// 9 is the "boundary=" length
	if (!boundary.empty() && (boundary[0] == '\"')) {
		//remove the double quotes
		boundary.erase(0, 1);
		boundary.erase(boundary.length() - 1, 1);
	}
	request.head["Boundary"] = boundary;
}

/**
 * @brief this function specify the body type ,and init the request map 
 * with appropriate elements [if we have a boundary it would be assigned ..]
 * if the type is none then we don't need to parse the request
*/
static BODY_TYPE	getTypeOfRequestBody(t_request& request) {
	std::string type;
	assignBoundary(request);

	if (request.head["Method"] == "POST")
	{
		type = request.head["Boundary"];
		if (!type.empty())
		{
			return BOUNDARY;
		}
		type = request.head["Transfer-Encoding"];
		if (type == "Chunked")
			return (CHUNKED);
		type = request.head["Content-Type"];
		if (type == "application/x-www-form-urlencoded") {
			return (SIMPLE);
		}
	}
	return (NONE);
}

/** @brief split the whole line by '\n'*/
std::vector<std::string>	getLines(std::string wholeLine) {
	std::vector<std::string> lines;
	std::string tmp;
	std::istringstream iss(wholeLine);

	while (std::getline(iss, tmp)) {
		size_t backRIndex = tmp.length() - 1;
		tmp.erase(backRIndex, 1);
	    lines.push_back(tmp);
    }
	return (lines);
}

void	parseMainHead(std::map<std::string, std::string> &headMap, std::vector<std::string> lines) {
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

static std::map<std::string, std::string>	parseHeader(t_request &request, std::string header, int typeOfHeader) {
	std::vector<std::string> 			lines;
	std::string 						key;
	std::string 						value;
	std::map<std::string, std::string> 	headMap;

	header += "\r\n";
	lines = getLines(header);
	size_t i = 0;
	if (typeOfHeader == HEADER_REQUEST)
	{
		parseMainHead(headMap, lines);
		i += 2;
	}

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
	return (headMap);
}


/**
 * @example will start from the line that contains the query params
*/
static void	parseSimpleBody(t_request &request, std::string body) {
	request.body["Query-String"] = body;
	// std::cout << request.request["Query-String"] << std::endl;
}

static void	parseBoundaryBody(t_request& request, std::string body) {
	std::string 	boundary;
	size_t			index;
	std::string		header;
	std::string		tmpString;
	bounderyBody	b;

	boundary	=	request.head["Boundary"];
	index		=	body.find(boundary + "\r\n");
	while (1) {
		index += boundary.size() + 2;
		if (index >= body.size())
			break ;
		tmpString = &body[index];
		header = tmpString.substr(0, tmpString.find("\r\n\r\n"));
		b.header = parseHeader(request, header, HEADER_BODY);
		index += header.size() + 4;
		if (index >= body.size())
			break ;
		tmpString = &body[index];
		b.postEntity = tmpString.substr(0, tmpString.find(boundary + "\r\n"));
		request.boundaries.push_back(b);
	}
}


static void	parseBody(t_request& request, std::string body) {
	switch (request.type) {
		case SIMPLE:
			parseSimpleBody(request, body);
			break;
		case BOUNDARY :
			parseBoundaryBody(request, body);
			break ;
		case CHUNKED :
			// parseChunkedBody();
			break;
		default:
			break;
	}
}




/* 
	This parsing based on these pillars
	1- HEADER
		/r/n/r/n
		BODY
	2 - The first line contains the METHOD PATH HTTPVERSION
	and the second one containg the HOST PORT
	and the rest would look like Key: Value
*/

void	printRequest(t_request request) {
	size_t i = 0;
	std::cout << request.boundaries.size() << std::endl;
	for (; i < request.boundaries.size(); i++) {
		std::map<std::string, std::string>::iterator it = request.boundaries[i].header.begin();
		for (; it != request.boundaries[i].header.end(); it++) {
			std::cout << "|"  << it->first << "|" << it->second << "|" << std::endl;
		}
		std::cout << request.boundaries[i].postEntity << std::endl;
	}
}


void 	requestParse(t_request& request, std::string buffer) {
	int	index = buffer.find("\r\n\r\n");
	std::string header = buffer.substr(0, index);
	std::string body = buffer.substr(index + 4, buffer.length());
	request.head = parseHeader(request, header, HEADER_REQUEST);
	request.type = getTypeOfRequestBody(request);
	parseBody(request, body);
	printRequest(request);
}