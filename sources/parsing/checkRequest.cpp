#include <Parsing.hpp>

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

void	checkRequest(std::string &buffer) {
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

void	checkPath(std::string path) {
	if (path.length() > 2048)
	{
		throw std::runtime_error("414 Request-URI Too Long");
	}
	if (path.find_first_not_of(ALLOWED_URI_CHARS) != std::string::npos)
		throw std::runtime_error("Bad char:400 Bad Request");
}
