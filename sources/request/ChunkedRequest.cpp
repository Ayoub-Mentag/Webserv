#include <ChunkedRequest.hpp>
#include <Utils.hpp>

ChunkedRequest::ChunkedRequest() {}

ChunkedRequest::ChunkedRequest(REQUEST_TYPE type) : Request(type) {}

void	ChunkedRequest::parseBody(std::string body) {
	std::vector<std::string> lines = splitLine(body, "\r\n");
	int							chunkSize;

	for (size_t index = 0; index < lines.size(); index++) {
		chunkSize = atoi(lines[index].c_str());
		if (index + 1) {
			/** @example in case if the content is larger that chunkSize 
			 * 1
			 * akjsdfkljsdlfkjasldfjldk\r\n
			*/
			this->body += lines[index + 1].substr(0, chunkSize);
		}
	}
}

std::string	ChunkedRequest::getBody() const {
	return this->body;
}

