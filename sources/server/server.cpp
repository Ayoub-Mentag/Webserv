#include <serverHeader.hpp>

void	correctPath(std::string& path) {
	// if (!path.empty() && path[0] != '.') {
	// 	path.insert(0, ".");
	// }

	DIR* dir = NULL;
	opendir(path.c_str());
	if (!dir)
	{
		if (path[path.length() - 1] == '/')
			path.erase(path.length() - 1);
		return ;
	}
	closedir(dir);
}

static std::string to_string(int num) {
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
			default:
				throw std::runtime_error(to_string(status) + " status code not handled");
		}
	}
	while (std::getline(os, line))
		result += line;
	return result;
}

std::string	directory_listing(DIR* dir, std::string root) {
	std::string response;
	response += "<html><body><ul>";

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		if (root[root.length() - 1] != '/') {
			root.insert(root.length() - 1, "/");
		}
		// std::cerr << root + std::string(entry->d_name) << std::endl;
		response += "<li><a href=\"" + root + std::string(entry->d_name) + "/\">" + std::string(entry->d_name) + "</a></li>";
	}
	// std::cerr << path << std::endl;
	response += "</ul></body></html>";
	return (response);
}

// void Server::sendFile(std::string fileName, std::string &response, t_request &request)
// {
// 	std::ifstream	inFile;
// 	std::string		send;

// 	inFile.open(fileName);
// 	if (inFile.fail()) {
// 		std::string errorPage = config.servers[request.serverIndex].locations[request.locationIndex].errorPages[400];
// 		inFile.open(errorPage);
// 		if (inFile.fail()) {
// 			errorPage = config.servers[request.serverIndex].errorPages[400];
// 			inFile.open(errorPage);
// 			if (inFile.fail())
// 				throw std::runtime_error(DEFAULT_ERROR_PAGE);
// 		}
// 	}
// 	while (std::getline(inFile, send))
// 		response += send;
// 	inFile.close();
// }

void Server::bindServerWithAddress()
{
	int result = bind(this->serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1)
	{
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

static int getLenOfMatching(std::string requestPath, std::string locationPath) {
	if (locationPath.size() > requestPath.size())
		return -1;
	int i = 0;
	while (i < (int)requestPath.size() && requestPath[i] == locationPath[i])
		i++;
	if (i == (int)locationPath.size() && (requestPath[i] == '/' || i == (int)requestPath.size()))
		return (i);
	return (-1);
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
	int serverIndex = request.serverIndex;
	std::vector<t_location> locations = config.servers[serverIndex].locations;
	// int i = -1;

	// int len = 0;
	// int tmp;

	// for (int j = 0; j < (int)locations.size(); j++)
	// {
	// 	tmp = getLenOfMatching(request.path, locations[j].path);
	// 	if (tmp > len)
	// 	{
	// 		len = tmp;
	// 		i = j;
	// 	}
	// }
	// if (i == -1)
	// 	throw std::runtime_error(DEFAULT_400_ERROR_PAGE);
	// request.locationIndex = i;
	/*
		/file.txt

		/				1 √
		/a				1
		/a/b			1
		/a/b/c			1 
	*/
}

std::string	Server::matching(t_request &request)
{
	std::vector<t_location> locations;
	std::string pathToBeLookFor;

	serverExists(request);
	if (request.serverIndex != -1) {
		locationExists(request);
		pathToBeLookFor = request.path;
	}
	


	// //if the path of the location is a directory
	// pathToBeLookFor.erase(0, locations[i].path.size());
	// std::string root = locations[i].root;
	// if (root.empty()) {
	// 	root = config.servers[z].root;
	// 	if (root.empty()) {
	// 		std::string body;
	// 		std::string	header;
	// 		try {
	// 			body = fileToString(config.servers[z].errorPages[NOT_FOUND_STATUS], NOT_FOUND_STATUS);
	// 		} catch(const std::exception& e) {
	// 			body = e.what();
	// 		}
	// 		header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	// 		throw std::runtime_error(header + body);
	// 	}
	// }
	// pathToBeLookFor.insert(0, root);
	// return (pathToBeLookFor);
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
		if (location.allowedMethods.size() == 0)
		{
			existInServer = std::find(server.allowedMethods.begin(), server.allowedMethods.end(), method) != server.allowedMethods.end();
			if (existInServer)
				return ;
			throw std::runtime_error(server.errorPages[405]);
		}
	}
	throw std::runtime_error(location.errorPages[405]);
}

void	Server::methodNotAllowed(t_request& request)
{
	t_server server = getServer(request.serverIndex);
	t_location location = getLocation(request.serverIndex, request.serverIndex);

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

void	Server::locationRedirection(std::string& path, t_request& request) {
	std::string	body;
	std::string header;
	t_location	location = getLocation(request.serverIndex, request.locationIndex);

	// correctPath(location.redirectFrom);
	// correctPath(location.redirectTo);
	std::cerr << "path: " << path << std::endl;
	std::cerr << "redirection From: " << location.redirectFrom << std::endl;
	std::cerr << "redirection to: " << location.redirectTo << std::endl;
	if (path == location.redirectFrom) {
		try {
			body = fileToString(location.redirectTo, NOT_FOUND_STATUS);
			header = request.httpVersion + "301 Moved Permanently\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} catch(const std::exception& e) {
			body = e.what();
			header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}
		throw std::runtime_error(header + body);
	}
}

void	Server::listDirectory(std::string& path, t_request& request) {
	t_location location = getLocation(request.serverIndex, request.locationIndex);
	DIR *dir = opendir(path.c_str());
	std::string	header;
	std::string	body;
	// std::cout << "path: " << path << std::endl;
	if (dir) {
		if (location.autoindex) {
			body = directory_listing(dir, request.path);
			header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} else if (!location.index.empty()) {
			try {
				// correctPath(location.index);
				body = fileToString(location.index, 404);
				header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
			} catch (const std::exception& ex) {
				body = ex.what();
				header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
			}
		} else {
			try {
				// correctPath(location.errorPages[FORBIDDEN_STATUS]);
				body = fileToString(location.errorPages[FORBIDDEN_STATUS], FORBIDDEN_STATUS);
			} catch (const std::exception& ex) {
				body = ex.what();
			}
			header = request.httpVersion + " 403 Forbidden\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}
		closedir(dir);
		throw std::runtime_error(header + body);
	}
}

void	Server::servFile(std::string& src, t_request& request) {
	std::string	header;
	std::string	body;

	if (access(src.c_str(), O_RDONLY) >= 0) {
		try {
			body = fileToString(src, 404);
			header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} catch(std::exception& ex) {
			body = ex.what();
			header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}
	}
	else {
		body = DEFAULT_404_ERROR_PAGE;
		header = request.httpVersion + "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	}
	throw std::runtime_error(header + body);
}

void Server::response(int clientFd, std::string src, t_request& request)
{
	std::string	response;

	try {
		src = matching(request);
		std::cout << request.serverIndex << " " << request.locationIndex << "\n";
		t_location location = getLocation(request.serverIndex, request.locationIndex);
		// correctPath(src);
		methodNotAllowed(request); // should i check location errpage first when no method in the location?
		locationRedirection(src, request);
		listDirectory(src, request);
		servFile(src, request);
	} catch (std::out_of_range &ofg) {
		(void)ofg;
	} catch (std::exception &ex) {
		response = ex.what();
	}
	write(clientFd, response.c_str(), response.length());
	close(clientFd);
	FD_CLR(clientFd, &current_sockets);
}

void Server::serve()
{
	fd_set readySocket = getReadyFds();
	std::string path;
	for (int clientFd = 0; clientFd < FD_SETSIZE; clientFd++)
	{
		if (FD_ISSET(clientFd, &readySocket))
		{
			if (clientFd == this->serverSocketfd) {
				acceptNewConnection();
			}
			else {
				t_request request = getRequest(clientFd);
				try {
					// path = matching(request); // throwing an exception ???
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
