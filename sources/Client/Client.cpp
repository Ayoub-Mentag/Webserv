#include <Client.hpp>
#include <Parsing.hpp>
#include <PostRequest.hpp>
#include <macros.hpp>

Client::Client(int fd) : fd(fd){
	statusHeadRead = false;
	request = NULL;
}

Request	*Client::getRequest() {
	return (this->request);
}

void	Client::setRequest(Request *request) {
	delete (request);
	this->request = request;
}

int	Client::getFd() const {
	return fd;
}

bool	Client::getStatusHeadRead() {
	return statusHeadRead;
}

void	Client::setStatusHeadRead(bool statusHeadRead) {
	this->statusHeadRead = statusHeadRead;
}

bool	Client::headEnds(std::string bufferLine) {
	size_t len = bufferLine.length();
	if (len < 4)
		return false ;
	bufferLine = bufferLine.substr(len - 4, len);
	return (bufferLine == "\r\n\r\n");
}



void	Client::initRequest() {
	/** @test we will work on some examples without getting the request from browser*/
	std::string	head;
	std::string	body;
	char		c;
	int			bytes;
	int			contentLength;
	std::map<std::string, std::string> headMap;

	while (1) {
		bytes = read(fd, &c, 1);
		if (bytes <= 0) {
			// Incomplete reading of head
			break ;
		}
		head += c;
		if (headEnds(head)) {
			statusHeadRead = true;
			break ;
		}
	}
	checkRequest(head);
	std::vector<std::string> lines = splitLine(head, "\r\n");
	if (lines.size() > 0)
		parseStartLine(headMap, lines[0]);
	parseHead(headMap, lines);
	checkPath(headMap[REQ_PATH]);
	if (headMap[REQ_METHOD] == "Post") {
		// TODO: what to do when the content len is > that the body that comes
		contentLength = atoi(request->getValueByKey(REQ_CONTENT_LENGTH).c_str());
		for (int i = 0; i < contentLength; i++) {
			bytes = read(fd, &c, 1);
			if (bytes <= 0)
				break ;
			body += c;
		}
		if (!headMap[REQ_BOUNDARY].empty())
			request = new BoundaryRequest(headMap);
		else 
			request = new PostRequest(headMap);
		parseBody(dynamic_cast<PostRequest *>(request));
	}
	else
		request = new DelGetRequest(headMap);

	// request = requestParse(bufferLine);

	/*testing the upload*/
	// PostRequest *p = dynamic_cast<PostRequest *>(request);
	// if (p) { 
	// 	BoundaryRequest *b = dynamic_cast<BoundaryRequest *>(p);
	// 	if (b) {
	// 		for (size_t i = 0; i < b->getData().size(); i++) {
	// 			Data d = b->getData()[i];
	// 			uploadFile(d);
	// 		}
	// 		// exit(0);		
	// 	}
	// }
	/*testing the upload*/
}