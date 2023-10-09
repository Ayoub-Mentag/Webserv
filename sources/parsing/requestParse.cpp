#include <parsingHeader.hpp>
 
void 	requestParse(t_request& request, std::string buffer) {
	std::istringstream	iss(buffer);
	std::string			str;


	request.locationIndex = -1;
	request.serverIndex = -1;
	// request line 

	std::getline(iss, str);
	size_t first = str.find_first_of(' ');
	size_t last = str.find_last_of(' ');
	request.method = str.substr(0, first);
	request.httpVersion = str.substr(last + 1,  -1);
	request.httpVersion = request.httpVersion.substr(0, request.httpVersion.length() - 1);
	request.path = str.substr(first + 1, last - first - 1);

	std::getline(iss, str);
	last = str.find_last_of(':');
	request.port = std::atoi(str.substr(last + 1, -1).c_str());
	request.serverName = str.substr(6, last - 6);
	// while (std::getline(iss, str)) {
	// }

	// GET /filesds HTTP/1.1

}