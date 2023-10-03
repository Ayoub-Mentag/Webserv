#include <serverHeader.hpp>

std::string	directory_listing(DIR* dir, std::string path) {
	std::string response;
	response += "<html><body><ul>";

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		response += "<li><a href=\"" + path + std::string(entry->d_name) + "/\">" + std::string(entry->d_name) + "</a></li>";
	}
	closedir(dir);
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
	while (i < (int)requestPath.size() & requestPath[i] == locationPath[i])
		i++;
	if (i == (int)locationPath.size() && (requestPath[i] == '/' || i == (int)requestPath.size()))
		return (i);
	return (-1);
}


std::string	Server::matching(t_request &request)
{
	int i = 0;
	int j = 0;
	int len = 0;
	int tmp;
	std::vector<t_location> locations;

	for (; i < (int)config.servers.size(); i++) {
		if (config.servers[i].serverName == request.serverName)
			break ;
	}
	if (i >= (int)config.servers.size())
		throw std::runtime_error("Server name not found");
	request.serverIndex = i;
	locations = config.servers[i].locations;
	i = -1;
	for (; j < (int)locations.size(); j++)
	{
		tmp = getLenOfMatching(request.path, locations[j].path);
		if (tmp > len)
		{
			len = tmp;
			i = j;
		}
	}
	if (i == -1)
		throw std::runtime_error("should I show 404 page or the root page");
	request.locationIndex = i;
	std::string pathToBeLookFor = request.path;
	pathToBeLookFor.erase(0, locations[i].path.size());
	pathToBeLookFor.insert(0, locations[i].root);
	return (pathToBeLookFor);
}

t_request	Server::getRequest(int clientFd) {
	t_request	request;
	char		buffer[MAX_LEN];

	bzero(buffer, MAX_LEN);
	recv(clientFd, buffer, MAX_LEN, 0);
	std::cerr << buffer << std::endl; 
	requestParse(request, buffer);
	return request;
}

static std::string to_string(int num) {
	std::stringstream	ss;

    ss << num;
	return (ss.str());
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
			default:
				throw std::runtime_error(to_string(status) + " status code not handled");
		}
	}
	while (std::getline(os, line))
		result += line;
	return result;
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
			header = request.httpVersion += "405 Method Not Allowed\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} catch(const std::exception& e) {
			body = e.what();
			header = request.httpVersion += "405 Method Not Allowed\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}
		throw std::runtime_error(header + body);
	}
}

void	Server::locationRedirection(std::string& path, t_request& request) {
	std::string	body;
	std::string header;
	t_location	location = getLocation(request.serverIndex, request.locationIndex);

	if (path == location.redirectFrom) {
		try {
			body = fileToString(location.redirectTo, NOT_FOUND_STATUS);
			header = request.httpVersion + "301 Moved Permanently\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} catch(const std::exception& e) {
			body = e.what();
			header = request.httpVersion += "404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}
		throw std::runtime_error(header + body);
	}
}

void	Server::listDirectory(std::string& path, t_request& request) {
	t_location location = getLocation(request.serverIndex, request.locationIndex);
	DIR *dir = opendir(path.c_str());
	std::string	header;
	std::string	body;


	if (dir) {
		if (location.autoindex) {
			body = directory_listing(dir, path);
			header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		}

		else if (!location.index.empty()) {
			body = fileToString(location.index);
			header = request.httpVersion + "200 OK\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
		} else {
			sendFile("403.html", response, request);
			ss << response.length();
			// header = " 403 Forbidden\r\nContent-type: text/html\r\nContent-length: ";
		}
	}
	else if ((fd = access(("." + src).c_str(), O_RDONLY)) >= 0) {
		sendFile("." + src, response, request);
	}
}

void Server::response(int clientFd, std::string src, t_request& request)
{
	std::string			response;
	// int fd ;
	(void)src;

	try {
		t_location location = getLocation(request.serverIndex, request.locationIndex);

		// 405 Method Not Allowed
		methodNotAllowed(request); // should i check location errpage first when no method in the location?
		locationRedirection(src, request);
		listDirectory(src, request);

	// 	else {
	// 		DIR *dir = opendir(("." + src).c_str());

	// 		if (dir) {
	// 			if (location.autoindex) {
	// 				if (src[src.length() -1] != '/')
	// 					src += '/';
	// 				response = directory_listing(dir, "." + src);
	// 			}
	// 			else if (!location.index.empty()) {
	// 				sendFile("." + location.index, response, request);		
	// 			} else if (location.index.empty()) {
	// 				sendFile("403.html", response, request);
	// 				ss << response.length();
	// 				// header = " 403 Forbidden\r\nContent-type: text/html\r\nContent-length: ";
	// 			}
	// 		}
	// 		else if ((fd = access(("." + src).c_str(), O_RDONLY)) >= 0) {
	// 			sendFile("." + src, response, request);
	// 		}
	// 		else
	// 		{
	// 			std::cerr << "." + src << " fd " << fd << std::endl;
	// 			std::cout << access(("." + src).c_str(), O_RDONLY) << std::endl;
	// 			throw std::runtime_error(DEFAULT_ERROR_PAGE);
	// 		}
	// 	}
	// 	ss << response.length();
	// 	std::string len = ss.str();
	// 	header = " 200 OK\r\nContent-type: text/html\r\nContent-length: ";		
	} catch (std::out_of_range &ofg) {
		(void)ofg;
	} catch (std::exception &ex) {
		response = ex.what();
		// std::cerr << response;
	}
	write(clientFd, response.c_str(), response.length());
	close(clientFd);
	FD_CLR(clientFd, &current_sockets);
}



//accept , response
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
					path = matching(request); // throwing an exception ???
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
