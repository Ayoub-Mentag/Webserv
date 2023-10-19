#include <Parsing.hpp>
#include <SimpleRequest.hpp>
#include <FormRequest.hpp>
#include <ChunkedRequest.hpp>

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

static bool	assignBoundary(std::map<std::string, std::string>& headMap) {
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
			headMap[REQ_METHOD]  = firstLine[0];
			headMap[REQ_PATH]    = firstLine[1];
			headMap[REQ_HTTP_VERSION] = firstLine[2];
		}
		std::istringstream iss2(lines[1]);
		while (std::getline(iss2, line, ':')) {
			secondLine.push_back(line);
		}
		
		size = secondLine.size();
		if (size >= 2) {
			headMap[REQ_SERVER_NAME] = trim(secondLine[1]);
			if (size == 3)
				headMap[REQ_PORT] = secondLine[2];
			else
				headMap[REQ_PORT] = "80";
		}
	}
}

// TODO: remove the (size_t i) param

void	parseHead(std::map<std::string, std::string>& headMap, std::vector<std::string> lines) {
	std::string	key;
	std::string	value;
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
	for (size_t i = 0; i < lines.size(); i++) {
		std::istringstream iss3(lines[i]);
		std::getline(iss3, key, ':');
		std::getline(iss3, value, ':');
		if (value[0] != ' ')
			throw std::runtime_error("Bad request");
		value.erase(0, 1);
		headMap[key] = value;
	}
	// for POST_BOUNDARY request
	assignBoundary(headMap);
}

/** @brief this function specify the body type ,and init the request map 
 * with appropriate elements [if we have a boundary it would be assigned ..]
 * if the type is none then we don't need to parse the request
*/

static REQUEST_TYPE	initTypeOfRequestBody(std::map<std::string, std::string>& headMap) {
	std::string type;
	type = headMap["Method"];
	if (type == "POST") {
		type = headMap["Transfer-Encoding"];
		if (!type.empty())
		{
			if (type == "chunked")
				return (POST_CHUNKED);
			throw std::runtime_error("Transfert-Encoding contains !Chunked:501 Not Implemented");
		}
		type = headMap["Content-Type"];
		if (type == "application/x-www-form-urlencoded")
			return (POST_SIMPLE);
		if (type == "application/json")
			return (POST_JSON);
		type = headMap["Boundary"];
		if (!type.empty())
			return (POST_BOUNDARY);

		// TODO: when we need to throw this
		// 	if (headMap["Content-Length"].empty())
		// throw std::runtime_error("Content-Length not exist:Bad request 400");

	} else if (type == "GET")
		return (GET);
	else if (type == "DELETE")
		return (DELETE);
	return (NONE);
}

static Request	*generateRequest(REQUEST_TYPE type) {
	switch (type) {
		case GET :
		case DELETE :
			return new SimpleRequest(type);
		case POST_BOUNDARY :
			return NULL;
			// return new BoundaryRequest(type);
		case POST_SIMPLE :
			return new FormRequest(type);
		case POST_CHUNKED : 
			return new ChunkedRequest(type);
		case POST_JSON : 
			return NULL;
		default :
			return NULL;
	}
}

static void removeSpaces(std::string &buffer, size_t index) {
	size_t		start;

	start = index;
	while (start < buffer.length() && buffer[start] == ' ')
		start++;
	if (start > index + 1)
		buffer.erase(index, start - index - 1);
}

static void	checkEnclosed(char c, std::stack<char> &myStack) {
	if (myStack.empty() || myStack.top() != c)
		myStack.push(c);
	else
		myStack.pop();
}

static void	checkLine(std::string& line, std::stack<char>& myStack, bool& quotes) {
	for (size_t j = 0; j < line.size(); j++) {
		if (line[j] == '\"' || line[j] == '\'')
		{
			quotes = !quotes;
			checkEnclosed(line[j], myStack);
		}
		else if (line[j] == ' ' && !quotes)  // remove extra spaces
			removeSpaces(line, j);
	}
}

static void	checkRequest(std::string &buffer) {
	// std::string correctRequest;
	std::stack<char>		myStack;
	std::string 			head;
	bool					quotes;

	quotes = false;
	size_t last = buffer.size() - 1;
	if (buffer[last] != '\n' || last - 1 < 0 || buffer[last - 1] != '\r')
		throw std::runtime_error("Bad end of request line:Bad Request 400");
	std::vector<std::string> lines = splitLine(buffer, "\r\n");
	buffer = "";
	for (size_t i = 0; i < lines.size(); i++) {
		checkLine(lines[i], myStack, quotes);
		buffer += (lines[i] + "\r\n");
	}

	/*
		index = 0;
		while (index < buffer.size()) {
			else if (buffer[index] == '\"')
			{
				quotes = !quotes;
				checkEnclosed(buffer[index], myStack);
			}
			else if (buffer[index] == ' ' && !quotes)  // remove extra spaces
				removeSpaces(buffer, index);
			index++;
		}
	*/
	if (myStack.size())
		throw std::runtime_error("something isn't ennclosed");
}

static void	checkPath(std::string path) {
	if (path.length() > 2048)
	{
		throw std::runtime_error("414 Request-URI Too Long");
	}
	if (path.find_first_not_of(ALLOWED_URI_CHARS) != std::string::npos)
		throw std::runtime_error("Bad char:400 Bad Request");
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

	checkRequest(buffer);

	index = buffer.find("\r\n\r\n");
	if (index == std::string::npos) {
		throw std::runtime_error("Bad request");
	}

	head 			= buffer.substr(0, index);
	lines 			= splitLine(head, "\r\n");
	// key: value
	parseTwoFirstLines(headMap, lines);
	lines.erase(lines.begin());
	lines.erase(lines.begin());
	parseHead(headMap, lines);
	checkPath(headMap[REQ_PATH]);


	request = generateRequest(initTypeOfRequestBody(headMap));
	if (!request)
		throw std::runtime_error("Unknown type of body:Bad Request");
	request->setHead(headMap);
	if (request->getTypeOfRequest() != DELETE && request->getTypeOfRequest() != GET)
	{
		body 	= buffer.substr(index + 4, buffer.length());
		request->parseBody(body);
	}
	return (request);
}