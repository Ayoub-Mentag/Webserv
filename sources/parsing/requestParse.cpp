#include <parsingHeader.hpp>

// void	correctPath(std::string& path) {
// 	// if (!path.empty() && path[0] != '.') {
// 	// 	path.insert(0, ".");
// 	// }

// 	DIR* dir = NULL;
// 	opendir(path.c_str());
// 	if (!dir) {
// 		if (path[path.length() - 1] == '/')
// 			path.erase(path.length() - 1);
// 		return ;
// 	}
// 	closedir(dir);
// }

// void 	requestParse(t_request& request, std::string buffer) {
// 	std::istringstream	iss(buffer);
// 	std::string			str;


// 	request.locationIndex = -1;
// 	request.serverIndex = -1;

// 	// request line 
// 	std::getline(iss, str);
// 	size_t first = str.find_first_of(' ');
// 	size_t last = str.find_last_of(' ');
// 	request.method = str.substr(0, first);
// 	request.httpVersion = str.substr(last + 1,  -1);
// 	request.httpVersion = request.httpVersion.substr(0, request.httpVersion.length() - 1);
// 	request.path = str.substr(first + 1, last - first - 1);
// 	correctPath(request.path);
// 	size_t dot = request.path.find_last_of('.');
// 	if (dot != request.path.npos) {
// 		std::string extention = request.path.substr(dot, -1);
// 		// if (extention[extention.length() - 1] == '/')
// 		// 	extention.erase(extention.length() - 1);
// 		request.contentType = fillContentTypeMap()[extention];
// 	}

// 	std::getline(iss, str);
// 	last = str.find_last_of(':');
// 	request.port = std::atoi(str.substr(last + 1, -1).c_str());
// 	request.serverName = str.substr(6, last - 6);
// 	// while (std::getline(iss, str)) {
// 	// }

// 	// GET /filesds HTTP/1.1

// }

enum BODY_TYPE{
	SIMPLE,
	BOUNDARY,
	CHUNKED,
	NONE
};


/**
 * @brief this function specify the body type ,and init the request map 
 * with appropriate elements [if we have a boundary it would be assigned ..]
 * if the type is none then we don't need to parse the request
*/

static BODY_TYPE	getTypeOfRequest(t_request& request) {
	// TODO: check if there are other types cuz it not handled yet
	// like "Content-Type: application/json"
	// add vector of QueryString
	std::string type;
	if (request.head["Method"] == "POST")
	{
		if (request.head.count("Boundary"))
			return BOUNDARY;
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


std::vector<std::string>	getLines(std::string wholeLine) {
	std::vector<std::string> lines;
	std::string line;
	std::istringstream iss(wholeLine);

	while (std::getline(iss, line)) {
		size_t backRIndex = line.length() - 1;
		line.erase(backRIndex, 1);
	    lines.push_back(line);
    }
	return (lines);
}

static void	assignBoundary(t_request &request) {
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
}

static std::map<std::string, std::string>	parseHeader(t_request &request, std::string header, int typeOfHeader) {
	std::vector<std::string> lines;
	std::string line;
    std::vector<std::string> firstLine;
    std::vector<std::string> secondLine;
	std::string 				key;
	std::string 				value;
	size_t						size;
	std::map<std::string, std::string> headMap;

	header += "\r\n";

	lines = getLines(header);
	size_t i = 0;
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
	std::string boundary = request.head["Boundary"];
	size_t index = body.find(boundary + "\r\n");
	std::string header;
	std::string postEntity;
	std::string tmpString;
	bounderyBody b;

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
		postEntity = tmpString.substr(0, tmpString.find(boundary + "\r\n"));
		b.postEntity = postEntity;
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
		std::cout << request.boundaries
		std::cout << request.boundaries[i].postEntity << std::endl;
	}
}


void 	requestParse(t_request& request, std::string buffer) {
	int	index = buffer.find("\r\n\r\n");
	std::string header = buffer.substr(0, index);
	std::string body = buffer.substr(index + 4, buffer.length());
	request.head = parseHeader(request, header, HEADER_REQUEST);
	assignBoundary(request);
	request.type = getTypeOfRequest(request);
	parseBody(request, body);

	printRequest(request);
}