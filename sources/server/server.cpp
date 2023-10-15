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
			case NOT_IMPLEMENTED_STATUS:
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

void	Server::bindServerWithAddress() {
	int result = bind(this->serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Server::setPortOfListening() {
	if (listen(serverSocketfd, 5) == -1)
	{
		throw std::runtime_error("listen");
	}
}

Server::Server(t_config& config) : config(config) {
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

Server::~Server() {
	close(serverSocketfd);
}

fd_set Server::getReadyFds() {
	fd_set ready_socket;
	FD_ZERO(&ready_socket);
	ready_socket = current_sockets;
	//read, write, error , timout
	if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL) < 0) {
		perror("Select : ");
		exit(1);
	}
	return ready_socket;
}

void	Server::acceptNewConnection() {
	struct sockaddr_in	clientAddr;
	int					clientFd;
	socklen_t			clientAddrLen;

	if ((clientFd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
		perror("Accept : ");
	} else {
		std::cout << "A new connection Accepted " << std::endl;
		FD_SET(clientFd, &current_sockets);
	}
}

void	Server::serverExists() {
	std::vector<t_server>	servers = config.servers;	

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

const std::string&	Server::returnError(int status) {
	if (request.serverIndex >= 0) {
		t_server server = getServer();
		if (request.locationIndex >= 0) {
			t_location location = getLocation();
			try {
				response.setBody(fileToString(location.errorPages[status], status));
			} catch(const std::exception& e) {
				try {
					response.setBody(fileToString(server.errorPages[status], status));
				} catch (const std::exception& ex) {
					response.setBody(ex.what());
				}
			}
		} else {
			try {
				response.setBody(fileToString(server.errorPages[status], status));
			} catch (const std::exception& ex) {
				response.setBody(ex.what());
			}
		}
	}

	response.setContentType(".html");
	response.setHeader(status);
	return (response.getResponse());
}

std::string	Server::matching() {
	serverExists(); // check error later!
	locationExists();
	if (request.locationIndex == -1) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}

	t_location	location = config.servers[request.serverIndex].locations[request.locationIndex];
	std::string	pathToBeLookFor = request.path;
	if (location.isCgi) {
		return (request.path);
	}
	location.isCgi = false;
	pathToBeLookFor.erase(0, location.path.size());
	if (!location.root.empty()) {
		pathToBeLookFor.insert(0, location.root);
	} else {
		pathToBeLookFor.insert(0, config.servers[request.serverIndex].root);
	}
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
	bool		existInLocation = false;
	bool		existInServer = false;
	std::string	header;

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

void	Server::methodNotAllowed() {
	t_server	server = getServer();
	t_location	location = getLocation();

	if (request.method != "GET" && request.method != "POST" && request.method != "DELETE") {
		throw std::runtime_error(returnError(NOT_IMPLEMENTED_STATUS));
	}
	try {
		findAllowedMethod(request.method, server, location);
	} catch (std::exception &ex) {
		throw std::runtime_error(returnError(METHOD_NOT_ALLOWED_STATUS));
	}
}

std::string	Server::locationRedirection() {
	t_location	location = getLocation();

	try {
		response.setBody(fileToString(location.redirectTo, NOT_FOUND_STATUS));
		response.setContentType(".html");
		response.setHeader(MOVED_PERMANENTLY_STATUS);
	} catch(const std::exception& e) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
	return (response.getResponse());
}

std::string	Server::listDirectory(DIR *dir) {
	t_location location = getLocation();
	
	// for now i will just throw forbidden!
	if (request.method == "DELETE") {
		throw std::runtime_error(returnError(FORBIDDEN_STATUS));
	}

	if (location.autoindex) {
		response.setContentType(".html");
		response.setBody(directory_listing(dir, request.path));
		response.setHeader(200);
	} else if (!location.index.empty()) {
		try {

			response.setContentType(".html");
			response.setBody(fileToString(location.index, NOT_FOUND_STATUS));
			response.setHeader(200);
		} catch (const std::exception& ex) {
			throw std::runtime_error(returnError(NOT_FOUND_STATUS));
		}
	} else {
		throw std::runtime_error(returnError(FORBIDDEN_STATUS));
	}
	closedir(dir);
	return (response.getResponse());
}

std::string	Server::servFile(std::string& path) {
	try {
		response.setBody(fileToString(path, NOT_FOUND_STATUS));
		response.setHeader(200);
	} catch(std::exception& ex) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
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

	response.setBody(body);
	response.setContentType(".html");
	response.setHeader(200);
	return (response.getResponse());
}

void	Server::parseContentType() {

	size_t dot = request.path.find_last_of('.');
	if (dot != request.path.npos) {
		if (request.path[request.path.length() - 1] == '/')
			request.path.erase(request.path.length() - 1);
		std::string extention = request.path.substr(dot, -1);
		response.setContentType(extention);
	} else {
		response.setContentType("");
	}
}

void	Server::initResponseClass() {
	parseContentType(); // setContentType
	response.setHttpVersion(request.httpVersion);
	response.setStatusCode();
}

// void	Server::deleteFile(std::string& path) {
// 	if (access(path.c_str(), F_OK) > -1) {
// 		if (std::remove(path.c_str()) == 0) {
// 			std::string tmp = "<!DOCTYPE html><html><head><title>Success</title></head><body><script>function showAlert() {alert(\"This is an alert message!\");}</script></body></html>";
// 			response.setBody(tmp);
// 			response.setHeader(200);
// 		} else {
// 			perror("remove");
// 			// throw std::runtime_error("probleme in deleting file\n");
// 		}
// 	} else {
// 		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
// 	}
// 	std::cout << response.getResponse();
// }

void Server::responseFunc(int clientFd)
{
	try {
		initResponseClass();
		std::string path = matching();
		t_location location = getLocation();
		correctPath(path);
		methodNotAllowed();

		// if (request.method == "DELETE") {
		// 	DIR *dir = opendir(path.c_str());
		// 	// if (path == location.redirectFrom) {
		// 	// 	locationRedirection();
		// 	if (dir) {
		// 		listDirectory(dir);
		// 	} else if (location.isCgi) {
		// 		executeCgi(path);
		// 	} else {
		// 		deleteFile(path);
		// 	}
		// }
		DIR *dir = opendir(path.c_str());
		if (path == location.redirectFrom) {
			locationRedirection();
		} else if (dir) {
			listDirectory(dir);
		} else if (location.isCgi) {
			executeCgi(path);
		} else {
			servFile(path);
		}
	} catch (std::out_of_range &ofg) {
		std::cout << __FILE__ << ":" << __LINE__ << std::endl;
		std::cout << ofg.what() << std::endl;
	} catch (std::exception &ex) {
		// just to catch the thrown error the response is already ready
	}
	write(clientFd, response.getResponse().c_str(), response.getResponse().length());
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
