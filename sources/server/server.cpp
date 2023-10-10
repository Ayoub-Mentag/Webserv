#include <serverHeader.hpp>

void	correctPath(std::string& path) {
	DIR* dir = NULL;
	opendir(path.c_str());
	if (!dir) {
		if (path[path.length() - 1] == '/')
			path.erase(path.length() - 1);
		return ;
	}
	closedir(dir);
}

std::string	to_string(int num) {
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
		result += line + "\n";
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

void	Server::serverExists() {
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

static size_t	findCgiLocation(t_request& request, t_location& location) {
	size_t index;

	index = location.path.find("*");
	if (index != std::string::npos) {
		std::string extention = &location.path[index + 1];
		size_t len = request.path.length();
		if (extention != &request.path[len - extention.length()]) {
			return std::string::npos;
		}
	}
	return index;
}

std::string	getCgiPath(t_request& request, t_location& location) {
	std::string lookFor = request.path;
	size_t	last = lookFor.find_last_of('/');
	std::string	path;

	if (last != lookFor.npos) {
		lookFor.erase(last, -1);
		if (location.root[0] == '.') {// what if the path contains ./ ../ 
			path = (&location.root[1] != lookFor) ? location.root + request.path : "." + request.path;
		} else {
			path = (location.root != lookFor) ? location.root + request.path : request.path;
		}
	}
	return (path);
}

void	Server::locationExists() {
	std::vector<t_location>	locations = config.servers[request.serverIndex].locations;
	size_t index;

	std::vector<std::string> locationPaths;
	for (size_t i = 0; i < locations.size(); i++) {
		index = findCgiLocation(request, locations[i]);
		if (index != std::string::npos) {
			std::string	cgiPath = getCgiPath(request, locations[i]);
			if (access(cgiPath.c_str(), F_OK) != -1) {
				request.locationIndex = i;
				config.servers[request.serverIndex].locations[i].isCgi = true;
				request.path = cgiPath;
				return ;
			}
		}
		locationPaths.push_back(locations[i].path);
		config.servers[request.serverIndex].locations[i].isCgi = false;
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

const std::string&	Server::returnError404() {
	if (request.serverIndex >= 0) {
		t_server server = getServer();
		if (request.locationIndex >= 0) {
			t_location location = getLocation();
			try {
				response.setBody(fileToString(location.errorPages[NOT_FOUND_STATUS], NOT_FOUND_STATUS));
			} catch(const std::exception& e) {
				try {
					response.setBody(fileToString(server.errorPages[NOT_FOUND_STATUS], NOT_FOUND_STATUS));
				} catch (const std::exception& ex) {
					response.setBody(ex.what());
				}
			}
		} else {
			try {
				response.setBody(fileToString(server.errorPages[NOT_FOUND_STATUS], NOT_FOUND_STATUS));
			} catch (const std::exception& ex) {
				response.setBody(ex.what());
			}
		}
	}

	response.setContentType("text/html");
	response.setHeader(NOT_FOUND_STATUS);
	return (response.getResponse());
}

std::string	Server::matching()
{
	serverExists(); // check error later!
	locationExists();
	if (request.locationIndex == -1) {
		throw std::runtime_error(returnError404());
	}

	t_location	location = config.servers[request.serverIndex].locations[request.locationIndex];
	std::string	pathToBeLookFor = request.path;
	if (location.isCgi) {
		return (request.path);
	}
	location.isCgi = false;
	pathToBeLookFor.erase(0, location.path.size());
	if (!location.root.empty())
		pathToBeLookFor.insert(0, location.root);
	else
		pathToBeLookFor.insert(0, config.servers[request.serverIndex].root);
	return (pathToBeLookFor);
}

void	Server::initRequest(int clientFd) {
	char		buffer[MAX_LEN];

	bzero(buffer, MAX_LEN);
	recv(clientFd, buffer, MAX_LEN, 0);
	std::cerr << buffer << std::endl;
	requestParse(request, buffer);

}

t_location&	Server::getLocation() {
	if (request.serverIndex < 0 || request.locationIndex < 0)
		throw std::out_of_range("getLocation()");
	return (config.servers[request.serverIndex].locations[request.locationIndex]);
}

t_server&	Server::getServer() {
	if (request.serverIndex < 0)
		throw std::out_of_range("getServer()");
	return (config.servers[request.serverIndex]);
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

const std::string&	Server::returnError501() {
	if (request.serverIndex >= 0) {
		t_server server = getServer();
		if (request.locationIndex >= 0) {
			t_location location = getLocation();
			try {
				response.setBody(fileToString(location.errorPages[NOT_IMPLEMENTED], NOT_IMPLEMENTED));
			} catch(const std::exception& e) {
				try {
					response.setBody(fileToString(server.errorPages[NOT_IMPLEMENTED], NOT_IMPLEMENTED));
				} catch (const std::exception& ex) {
					response.setBody(ex.what());
				}
			}
		} else {
			try {
				response.setBody(fileToString(server.errorPages[NOT_IMPLEMENTED], NOT_IMPLEMENTED));
			} catch (const std::exception& ex) {
				response.setBody(ex.what());
			}
		}
	}

	response.setContentType("text/html");
	response.setHeader(NOT_IMPLEMENTED);
	return (response.getResponse());
}

const std::string&	Server::returnError405() {
	if (request.serverIndex >= 0) {
		t_server server = getServer();
		if (request.locationIndex >= 0) {
			t_location location = getLocation();
			try {
				response.setBody(fileToString(location.errorPages[METHOD_NOT_ALLOWED_STATUS], METHOD_NOT_ALLOWED_STATUS));
			} catch(const std::exception& e) {
				try {
					response.setBody(fileToString(server.errorPages[METHOD_NOT_ALLOWED_STATUS], METHOD_NOT_ALLOWED_STATUS));
				} catch (const std::exception& ex) {
					response.setBody(ex.what());
				}
			}
		} else {
			try {
				response.setBody(fileToString(server.errorPages[METHOD_NOT_ALLOWED_STATUS], METHOD_NOT_ALLOWED_STATUS));
			} catch (const std::exception& ex) {
				response.setBody(ex.what());
			}
		}
	}

	response.setContentType("text/html");
	response.setHeader(METHOD_NOT_ALLOWED_STATUS);
	return (response.getResponse());
}

void	Server::methodNotAllowed() {
	t_server server = getServer();
	t_location location = getLocation();

	if (request.method != "GET" && request.method != "POST" && request.method != "DELETE") {
		throw std::runtime_error(returnError501());
	}
	try {
		findAllowedMethod(request.method, server, location);
	} catch (std::exception &ex) {
		throw std::runtime_error(returnError405());
	}
}

std::string	Server::locationRedirection() {
	t_location	location = getLocation();

	try {
		response.setBody(fileToString(location.redirectTo, NOT_FOUND_STATUS));
		response.setContentType("text/html");
		response.setHeader(MOVED_PERMANENTLY_STATUS);
	} catch(const std::exception& e) {
		throw std::runtime_error(returnError404());
	}
	return (response.getResponse());
}

const std::string&	Server::returnError403() {
	if (request.serverIndex >= 0) {
		t_server server = getServer();
		if (request.locationIndex >= 0) {
			t_location location = getLocation();
			try {
				response.setBody(fileToString(location.errorPages[FORBIDDEN_STATUS], FORBIDDEN_STATUS));
			} catch(const std::exception& e) {
				try {
					response.setBody(fileToString(server.errorPages[FORBIDDEN_STATUS], FORBIDDEN_STATUS));
				} catch (const std::exception& ex) {
					response.setBody(ex.what());
				}
			}
		} else {
			try {
				response.setBody(fileToString(server.errorPages[FORBIDDEN_STATUS], FORBIDDEN_STATUS));
			} catch (const std::exception& ex) {
				response.setBody(ex.what());
			}
		}
	}

	response.setContentType("text/html");
	response.setHeader(FORBIDDEN_STATUS);
	return (response.getResponse());
}

std::string	Server::listDirectory(DIR *dir) {
	t_location location = getLocation();
	
	if (location.autoindex) {
		response.setBody(directory_listing(dir, request.path));
		response.setHeader(200);
	} else if (!location.index.empty()) {
		try {
			response.setBody(fileToString(location.index, NOT_FOUND_STATUS));
			response.setHeader(200);
		} catch (const std::exception& ex) {
			throw std::runtime_error(returnError404());
		}
	} else {
		throw std::runtime_error(returnError403());
	}
	closedir(dir);
	return (response.getResponse());
}

std::string	Server::servFile(std::string& path) {
	// if (access(path.c_str(), O_RDONLY) >= 0) {
	try {
		response.setBody(fileToString(path, NOT_FOUND_STATUS));
		response.setHeader(200);
	} catch(std::exception& ex) {
		throw std::runtime_error(returnError404());
	}
	// }
	// else {
	// 	body = DEFAULT_404_ERROR_PAGE;
	// 	header = request.httpVersion + " 404 Not Found\r\nContent-type: text/html\r\nContent-length: " + to_string(body.length()) + " \r\n\r\n";
	
	// }
	return (response.getResponse());
}

std::string	Server::executeCgi(std::string path) {
	std::string body;
	char		*program[3];
	pid_t pid;
	int			fd[2];
	char		buffer[MAX_LEN];
	t_location	location = config.servers[request.serverIndex].locations[request.locationIndex];

	program[0] = (char *)location.cgiExecutable.c_str();
	program[1] = (char *)path.c_str();
	program[2] = NULL;
	pipe(fd);
	pid = fork();
	if (pid == 0) {
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		execve(program[0], program, NULL);
		perror("Execve");
	}
	wait(0);
	bzero(buffer, MAX_LEN);
	read(fd[0], buffer, MAX_LEN);
	close(fd[0]);
	close(fd[1]);
	body = buffer;
	body = body.substr(body.find("\r\n\r\n"), -1);

	std::string	header = request.httpVersion;
	header += " 200 OK";
	header += "\r\nContent-type: text/html\r\nContent-length: ";
	header += to_string(body.length());
	header += " \r\n\r\n";

	// header = response.getHttpVersion();
	// header += response.getStatusCode[200];

	return (header + body);
}

void	getContentType(std::string& path, t_response& response) {

	size_t dot = path.find_last_of('.');
	if (dot != path.npos) {
		if (path[path.length() - 1] == '/')
			path.erase(path.length() - 1);
		std::string extention = path.substr(dot, -1);
	// std::cout << fillContentTypeMap()[extention] << "<---2" << std::endl;
		response.setContentType(fillContentTypeMap()[extention]);
	}
}

void	Server::initResponseClass(std::string& path) {
	getContentType(path, response); // setContentType
	// std::cout << response.getContentType() << std::endl;
	response.setHttpVersion(request.httpVersion);
	response.setStatusCode();
}

void Server::responseFunc(int clientFd)
{
	std::string	_response;

	try {
		initResponseClass(request.path);
		std::string path = matching();
		t_location location = getLocation();
		correctPath(path);
		DIR *dir = opendir(path.c_str());
		methodNotAllowed(); // should i check location errpage first when no method in the location?
		if (path == location.redirectFrom) {
			_response = locationRedirection();
		} else if (dir) {
			_response = listDirectory(dir);
		} else if (location.isCgi) {
			_response = executeCgi(path);
		} else {
			_response = servFile(path);
		}
	} catch (std::out_of_range &ofg) {
		(void)ofg;
	} catch (std::exception &ex) {
		_response = ex.what();
	}
	write(clientFd, _response.c_str(), _response.length());
	close(clientFd);
	FD_CLR(clientFd, &current_sockets);
}

void Server::serve()
{
	fd_set readySocket = getReadyFds();
	for (int clientFd = 0; clientFd < FD_SETSIZE; clientFd++) {
		if (FD_ISSET(clientFd, &readySocket)) {
			if (clientFd == this->serverSocketfd) {
				acceptNewConnection();
			} else {
				initRequest(clientFd);
				try {
					responseFunc(clientFd);

				} catch(const std::exception& e) { // not handled !!!!!!!!
					std::cout << e.what() << std::endl;
				}
				close(clientFd);
				FD_CLR(clientFd, &current_sockets);
			}
		}
	}
}
