#include <Parsing.hpp>
#include <DelGetRequest.hpp>
#include <BoundaryRequest.hpp>
#include <PostRequest.hpp>
#include <Utils.hpp>
#include <macros.hpp>
/** @brief
	This parsing based on these pillars
	1- HEADER
		/r/n/r/n
		BODY
	2 - The first line contains the METHOD PATH HTTPVERSION
	and the second one containg the HOST PORT
	and the rest would look like Key: Value
*/
 

void	parseStartLine(std::map<std::string, std::string> &headMap, std::string line) {
	std::vector<std::string>	firstLine;
	std::vector<std::string>	splittedPath;
	
	std::istringstream iss1(line);
	while (std::getline(iss1, line, ' ')) {
		firstLine.push_back(line);
	}
	if (firstLine.size() == 3) {
		headMap[REQ_METHOD]  = trim(firstLine[0]);
		if (firstLine[1].find("?") != std::string::npos) {
			splittedPath = splitLine(firstLine[1], "?");
			headMap[REQ_PATH] = splittedPath[0];
			headMap[REQ_QUERY_STRING] = splittedPath[1];
		}
		else
			headMap[REQ_PATH]    = trim(firstLine[1]);
		headMap[REQ_HTTP_VERSION] = trim(firstLine[2]);
	}
}

// TODO: remove the (size_t i) param

void	setKeyAndValue(std::string line, std::map<std::string, std::string>& headMap) {
	std::string key, value;

	std::istringstream iss(line);
	char delimiter;
	if (line.find(":") != std::string::npos) {
		delimiter = ':';
	}
	else if (line.find("=") != std::string::npos) {
		delimiter = '=';
	}
	else
		throw std::runtime_error("Bad request");
	std::getline(iss, key, delimiter);
	key = trim(key);
	value = &line[key.length() + 2];
	if (key.empty() || value.empty())
		throw std::runtime_error("EMPTY");

	headMap[key] = value;
	if (key == "Host") {
		value = headMap[key];//localhost:8080
		headMap.erase(key);
		key = REQ_SERVER_NAME;
		std::istringstream iss(value);
		std::string servername, port;
		std::getline(iss, servername, delimiter);
		std::getline(iss, port, delimiter);
		headMap[REQ_SERVER_NAME] = servername;
		if (!port.empty()) {
			headMap[REQ_PORT] = port;
		}
	}
	else 
	{
		headMap[key] = value;
	}
}

void	parseHead(std::map<std::string, std::string>& headMap, std::vector<std::string> lines) {
	std::string					key;
	std::string					value;
	std::vector<std::string>	valuesWithinLine;

	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i].find(";"))
			valuesWithinLine = splitLine(lines[i], ";");
		else 
			valuesWithinLine.push_back(lines[i]);
		for (size_t j = 0; j < valuesWithinLine.size(); j++) {
			try {
				setKeyAndValue(valuesWithinLine[j], headMap);
			} catch(std::exception &e) {
				e.what();
			}
		}
	}
}

/** @brief this function specify the body type ,and init the request map 
 * with appropriate elements [if we have a boundary it would be assigned ..]
 * if the type is none then we don't need to parse the request
*/


// R`

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

void	checkRequest(std::string	buffer) {
	// std::string correctRequest;
	std::stack<char>		myStack;
	std::string 			head;
	bool					quotes;

	quotes = false;
	// size_t last = buffer.size() - 1;
	// if (buffer[last] != '\n' || last - 1 < 0 || buffer[last - 1] != '\r')
	// 	throw std::runtime_error("Bad end of request line:Bad Request 400");
	std::vector<std::string> lines = splitLine(buffer, "\r\n");
	buffer = "";
	for (size_t i = 0; i < lines.size(); i++) {
		checkLine(lines[i], myStack, quotes);
		buffer += (lines[i] + "\r\n");
	}
	if (myStack.size())
		throw std::runtime_error("something isn't ennclosed");
}

void	checkPath(std::string path) {
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


void	parseBoundary(BoundaryRequest *b, std::string body, std::string boundary) {
	std::string							head;
	std::string							content;
	int 								i;
	std::map<std::string, std::string>	headOfBody;

	i = 0;
	boundary.insert(0, "--");
	while (body.find(boundary) != std::string::npos) {
		body = &body[boundary.size()];
		if (body.empty() || body == "--\r\n")
			break;
		head = body.substr(0, body.find("\r\n\r\n") + 4);
		body = &body[head.size()];
		if (body.empty())
			break ;
		parseHead(headOfBody, splitLine(head, "\r\n"));
		content = body.substr(0, body.find(boundary) - 2);
		body = &body[content.size() + 2];
		b->setDataByValues(headOfBody, content);
		i++;
	}
}


std::string	parseChunked(std::string body) {
	std::vector<std::string> lines;
	int chunkSize;
	std::string content;

	lines = splitLine(body, "\r\n");
	for (size_t j = 0; j < lines.size(); j++) {
		chunkSize = atoi(lines[j++].c_str());
		while (chunkSize) {
			content += lines[j];
			chunkSize -= lines[j].size();
		}
	}
	return (content);
}


void	parseBody(PostRequest *post) {
	size_t		i;
	std::string	content;
	bool		chunked = post->getValueByKey(REQ_TRANSFER) == "chunked" ? true : false;

	std::cout << post->getBody() << std::endl;
	BoundaryRequest *b = dynamic_cast<BoundaryRequest *>(post);
	if (b) {
		parseBoundary(b, post->getBody(), post->getValueByKey(REQ_BOUNDARY));
		if (chunked) {
			i = 0;
			for (; i < b->getData().size(); i++) {
				content = parseChunked(b->getDataByIndex(i).getContent());
				b->setContentByIndex(content, i);
			}
		}
	}
	else if (chunked) {
		post->setBody(parseChunked(post->getBody()));
	}

	// print the content
	i = 0;
	if (b) {
		for (; i < b->getData().size(); i++) {
			Data tmp = b->getDataByIndex(i);
			printMap(tmp.getHead());
			std::cerr << tmp.getContent();
		}
	}
	else 
		std::cerr << post->getBody() << std::endl;
}



// Request 	*requestParse(std::string buffer) {
// 	size_t								index;
// 	std::string							head;
// 	std::string							body;
// 	Request								*request;
// 	std::map<std::string, std::string >	headMap;
// 	std::vector<std::string>			lines;
// 	std::vector<std::string>			data;

// 	checkRequest(buffer);

// 	index = buffer.find("\r\n\r\n");
// 	if (index == std::string::npos) {
// 		throw std::runtime_error("Bad request");
// 	}
// 	head 			= buffer.substr(0, index);
// 	lines 			= splitLine(head, "\r\n");
// 	// key: value
// 	parseStartLine(headMap, lines);
// 	lines.erase(lines.begin());
// 	parseHead(headMap, lines);
// 	checkPath(headMap[REQ_PATH]);

// 	request = generateRequest(headMap);
// 	request->setHead(headMap);
// 	if (!request)
// 		throw std::runtime_error("Unknown type of body:Bad Request");
// 	if (request->getTypeOfRequest() != DELETE && request->getTypeOfRequest() != GET)
// 	{
// 		// exit(1);
// 		PostRequest *p = dynamic_cast<PostRequest *>(request);
// 		p->setBody(buffer.substr(index + 4, buffer.length()));
// 		parseBody(p);
// 	}
// 	return (request);
// }

