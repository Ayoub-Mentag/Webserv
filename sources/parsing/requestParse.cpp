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

static void	parseHeader(t_request& request, std::string header) {
	std::istringstream iss;
	std::vector<std::string> lines;
	std::string line;
    std::vector<std::string> firstLine;
    std::vector<std::string> secondLine;
	std::string 				key;
	std::string 				value;

	iss.str(header);
	while (std::getline(iss, line)) {
		line.replace(line.length() - 1, 1, "\n");
	    lines.push_back(line);
    }

	if (lines.size() > 2) {
		std::istringstream iss1(lines[0]);
		while (std::getline(iss1, line, ' ')) {
			firstLine.push_back(line);
		}
		if (firstLine.size() == 3) {
			request.request["Method"]  = firstLine[0];
			request.request["PATH"]    = firstLine[1];
			request.request["HTTPVER"] = firstLine[2];
		}

		std::istringstream iss2(lines[1]);
		while (std::getline(iss2, line, ':')) {
			secondLine.push_back(line);
		}
		if (firstLine.size() == 3) {
			request.request["Host"]  = secondLine[1];
			request.request["Port"]    = secondLine[2];
		}
		// std::cout << request.request["Method"] << " " << request.request["Port"] << " " << request.request["HTTPVER"] << std::endl;
		// std::cout << request.request["Host"] << " " << request.request["Port"] << std::endl;
	}


	for (size_t i = 2; i < lines.size(); i++) {
		std::istringstream iss3(lines[i]);
		std::getline(iss3, key, ':');
		std::getline(iss3, value, ':');
		value.erase(0, 1);
		value.erase(value.length() - 1, 1);
		request.request[key] = value;
		// std::cout << key << " |" << value << "|" << std::endl;
	}
}

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

static BODY_TYPE	checkBody(t_request& request, std::string& body) {
	std::string type;
	if (request.request["Methdo"] == "Post")
	{
		type = request.request["Transfer-Encoding"];
		if (type.empty())
			return NONE;

		if (type == "Chunked")
			return (CHUNKED);
		type = request.request["Content-Type"];
		if (type.empty())
			return NONE;
		if (type == "multipart/form-data") {
			return (SIMPLE);
		}
		// TODO: check if there are other types cuz it not handled yet
		request.request["Content-Type"] = "multipart/form-data";
		//Content-Type: multipart/form-data; boundary="-----BOUNDARY"
		std::string boundary = type.erase(type.find(type.find("boundary="), 9));// 9 is the "boundary=" length
		if (!boundary.empty() && (boundary[0] == '\"'))
 		{
			//remove the double quotes
			boundary.erase(0, 1);
			boundary.erase(boundary.length() - 1, 1);
		}
		request.request["Boundary"] = boundary;
		return (BOUNDARY);
	}
	return (NONE);
}

static	void	parseSimpleBody() {
	
}

static void	parseBody(t_request& request, std::string body) {
	BODY_TYPE type = checkBody(request, body);
	switch (type)
	{
	case SIMPLE:
		parseSimpleBody();
		break;
	case BOUNDARY : 
		parseBoundaryBody();
		break ;
	case CHUNKED :
		parseChunkedBody();
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
void 	requestParse(t_request& request, std::string buffer) {
	int	index = buffer.find("/r/n/r/n");
	std::string header = buffer.substr(0, index);
	std::string body = buffer.substr(index + 4, buffer.length());
	std::cout << header << std::endl;
	parseHeader(request, header);
	parseBody(request, body);
}