#include <serverHeader.hpp>

void	correctPath(std::string& path) {
	// if (!path.empty() && path[0] != '.') {
	// 	path.insert(0, ".");
	// }

	DIR* dir = NULL;
	opendir(path.c_str());
	if (!dir) {
		if (path[path.length() - 1] == '/')
			path.erase(path.length() - 1);
		return ;
	}
	closedir(dir);
}

static std::string	to_string(int num) {
	std::stringstream	ss;

    ss << num;
	return (ss.str());
}

std::string	fileToString(std::string fileName, int status) {
	std::string result;
	std::string line;
	std::ifstream os;

	os.open(fileName);
	if (os.fail()) {
		switch (status) {
			case METHOD_NOT_ALLOWED_STATUS:
				throw std::runtime_error(DEFAULT_405_ERROR_PAGE);
			case NOT_FOUND_STATUS:
				throw std::runtime_error(DEFAULT_404_ERROR_PAGE);
			case FORBIDDEN_STATUS:
				throw std::runtime_error(DEFAULT_403_ERROR_PAGE);
			case BAD_REQUEST_STATUS:
				throw std::runtime_error(DEFAULT_400_ERROR_PAGE);
			case NOT_IMPLEMENTED:
				throw std::runtime_error(DEFAULT_501_ERROR_PAGE);
			default:
				throw std::runtime_error(to_string(status) + " status code not handled");
		}
	}
	while (std::getline(os, line)) {
		result += line;
	}
	return (result);
}

std::string	directory_listing(DIR* dir, std::string root) {
	std::string response;
	response += "<html><body><ul>";

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		if (root[root.length() - 1] != '/') {
			root.insert(root.length(), "/");
		}
		response += "<li><a href=\"" + root + std::string(entry->d_name) + "/\">" + std::string(entry->d_name) + "</a></li>";
	}
	response += "</ul></body></html>";
	return (response);
}

void Server::bindServerWithAddress()
{
	int result = bind(this->serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::setPortOfListening()
{
	if (listen(serverSocketfd, 5) == -1)
	{
		throw std::runtime_error("listen");
	}
}

Server::Server(t_config& config) : config(config) 
{
	if ((this->serverSocketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket() : ");
		throw std::runtime_error("socket()");
	}
	int opt = 1;
	if (setsockopt(this->serverSocketfd, SOL_SOCKET,  SO_REUSEPORT , &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);
	FD_ZERO(&current_sockets);
	FD_SET(serverSocketfd, &current_sockets);
	bindServerWithAddress();
	setPortOfListening();
}

Server::~Server()
{
	close(serverSocketfd);
}

fd_set Server::getReadyFds() {
	fd_set ready_socket;
	FD_ZERO(&ready_socket);
	ready_socket = current_sockets;
	//read, write, error , timout
	if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL) < 0)
	{
		perror("Select : ");
		exit(1);
	}
	return ready_socket;
}

void Server::acceptNewConnection()
{
	int clientFd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;
	if ((clientFd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
		perror("Accept : ");
	}
	else {
		std::cout << "A new connection Accepted " << std::endl;
		FD_SET(clientFd, &current_sockets);
	}
}

void	Server::serverExists(t_request& request) {
	std::vector<t_server> servers = config.servers;	

	for (int serverIndex = 0; serverIndex < (int)config.servers.size(); serverIndex++) {
		if (servers[serverIndex].serverName == request.serverName
			&& request.port == servers[serverIndex].port) {
			request.serverIndex = serverIndex;
			return ;
		}
	}
	request.serverIndex = -1;
	//error page : SERVER_NOT_FOUND
}

void	Server::locationExists(t_request& request) {
	std::vector<t_location>	locations = config.servers[request.serverIndex].locations;

	std::vector<std::string> locationPaths;
	for (size_t i = 0; i < locations.size(); i++) {
		locationPaths.push_back(locations[i].path);
	}
	
	std::string lookFor = request.path;
	while (lookFor.size() > 0) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), lookFor);
		if (it != locationPaths.end()) {
			request.locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
		size_t	last = lookFor.find_last_of('/');
		if (last == lookFor.npos)
			break ;
		lookFor.erase(last, -1);
	}
	if (request.path[0] == '/' && lookFor.empty()) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), "/");
		if (it != locationPaths.end()) {
			request.locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
	}
}

std::string	Server::matching(t_request &request)
{
	serverExists(request); // check error later!
	locationExists(request);
	if (request.locationIndex == -1) {
		std::string body;
		try {
			body = fileToString(config.servers[request.serverIndex].errorPages[NOT_FOUND_STATUS], NOT_FOUND_STATUS);
		} catch (const std::exception& ex) {
			body = ex.what();
		}
		std::string header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		throw std::runtime_error(header + body);
	}

	t_location	location = config.servers[request.serverIndex].locations[request.locationIndex];
	std::string	pathToBeLookFor = request.path;

	pathToBeLookFor.erase(0, location.path.size());
	if (!location.root.empty())
		pathToBeLookFor.insert(0, location.root);
	else
		pathToBeLookFor.insert(0, config.servers[request.serverIndex].root);
	return (pathToBeLookFor);
}

t_request	Server::getRequest(int clientFd) {
	t_request	request;
	char		buffer[MAX_LEN];

	bzero(buffer, MAX_LEN);
	recv(clientFd, buffer, MAX_LEN, 0);
	// std::cerr << buffer << std::endl;
	requestParse(request, buffer);
	return request;
}

t_location&	Server::getLocation(int serverIndex, int locationIndex) {
	if (serverIndex < 0 || locationIndex < 0)
		throw std::out_of_range("getLocation()");
	return (config.servers[serverIndex].locations[locationIndex]);
}

t_server&	Server::getServer(int serverIndex) {
	if (serverIndex < 0)
		throw std::out_of_range("getServer()");
	return (config.servers[serverIndex]);
}

void	findAllowedMethod(std::string& method, t_server& server, t_location& location) {
	bool existInLocation = false;
	bool existInServer = false;
	std::string header;

	existInLocation = std::find(location.allowedMethods.begin(), location.allowedMethods.end(), method) != location.allowedMethods.end();
	if (existInLocation) {
		return ; // means that the method is allowed
	} else {
		if (location.allowedMethods.size() == 0) {
			existInServer = std::find(server.allowedMethods.begin(), server.allowedMethods.end(), method) != server.allowedMethods.end();
			if (existInServer)
				return ;
			throw std::runtime_error(server.errorPages[405]);
		}
		throw std::runtime_error(location.errorPages[405]);
	}
}

void	Server::methodNotAllowed(t_request& request) {
	t_server server = getServer(request.serverIndex);
	t_location location = getLocation(request.serverIndex, request.locationIndex);

	if (request.method != "GET" && request.method != "POST" && request.method != "DELETE") {
		std::string body;
		std::string header;
		try {
			body = fileToString(location.errorPages[NOT_IMPLEMENTED], NOT_IMPLEMENTED);
		} catch(const std::exception& e) {
			body = e.what();
		}
		header = request.httpVersion += "501 Not Implemented\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		throw std::runtime_error(header + body);
	}
	try {
		findAllowedMethod(request.method, server, location);
	} catch (std::exception &ex) {
		std::string body;
		std::string header;
		try {
			body = fileToString(ex.what(), METHOD_NOT_ALLOWED_STATUS);
		} catch(const std::exception& e) {
			body = e.what();
		}
		header = request.httpVersion += "405 Method Not Allowed\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		throw std::runtime_error(header + body);
	}
}

std::string	Server::locationRedirection(t_request& request) {
	std::string	body;
	std::string header;
	t_location	location = getLocation(request.serverIndex, request.locationIndex);

	try {
		body = fileToString(location.redirectTo, NOT_FOUND_STATUS);
		header = request.httpVersion + "301 Moved Permanently\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	} catch(const std::exception& e) {
		body = e.what();
		header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		throw std::runtime_error(header + body);
	}
	return (header + body);
}

std::string	Server::listDirectory(t_request& request, DIR *dir) {
	t_location location = getLocation(request.serverIndex, request.locationIndex);
	
	std::string	header;
	std::string	body;
	if (location.autoindex) {
		body = directory_listing(dir, request.path);
		header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	} else if (!location.index.empty()) {
		try {
			body = fileToString(location.index, 404);
			header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} catch (const std::exception& ex) {
			body = ex.what();
			header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}
		throw std::runtime_error(header + body);
	} else {
		try {
			body = fileToString(location.errorPages[FORBIDDEN_STATUS], FORBIDDEN_STATUS);
		} catch (const std::exception& ex) {
			body = ex.what();
		}
		header = request.httpVersion + " 403 Forbidden\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		throw std::runtime_error(header + body);
	}
	closedir(dir);
	return (header + body);
}


std::string	Server::servFile(std::string& src, t_request& request) {
	std::string	header;
	std::string	body;

	// if (access(src.c_str(), O_RDONLY) >= 0) {
	try {
		body = fileToString(src, 404);
		header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	} catch(std::exception& ex) {
		body = ex.what();
		header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		throw std::runtime_error(header + body);
	}
	// }
	// else {
	// 	body = DEFAULT_404_ERROR_PAGE;
	// 	header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	
	// }
	return (header + body);
}

void Server::response(int clientFd, std::string src, t_request& request)
{
	std::string	response;

	try {
		src = matching(request);
		t_location location = getLocation(request.serverIndex, request.locationIndex);
		correctPath(src);
		DIR *dir = opendir(src.c_str());
		methodNotAllowed(request); // should i check location errpage first when no method in the location?
		if (src == location.redirectFrom) {
			response = locationRedirection(request);
		} else if (dir) {
			// std::cout << "---- here ----\n";
			response = listDirectory(request, dir);
		} else {
			response = servFile(src, request);
		}
	} catch (std::out_of_range &ofg) {
		(void)ofg;
	} catch (std::exception &ex) {
		response = ex.what();
	}
	// std::cout << response << std::endl;
	write(clientFd, response.c_str(), response.length());
	close(clientFd);
	FD_CLR(clientFd, &current_sockets);
}

void Server::serve()
{
	fd_set readySocket = getReadyFds();
	std::string path;
	for (int clientFd = 0; clientFd < FD_SETSIZE; clientFd++) {
		if (FD_ISSET(clientFd, &readySocket)) {
			if (clientFd == this->serverSocketfd) {
				acceptNewConnection();
			} else {
				t_request request = getRequest(clientFd);
				try {
					// path = matching(request);
					response(clientFd, path, request);

				} catch(const std::exception& e) { // not handled !!!!!!!!
					std::cout << e.what() << std::endl;
				}
				close(clientFd);
				FD_CLR(clientFd, &current_sockets);
			}
		}
	}
}
