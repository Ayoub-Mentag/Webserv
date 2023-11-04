#include <Server.hpp>
#include <Utils.hpp>
#include <BoundaryRequest.hpp>
#include <PostRequest.hpp>
#include <DelGetRequest.hpp>
#include <Client.hpp>

//TODO: move the response code to the response


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

static std::string	directory_listing(DIR* dir, std::string root) {
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
	if (result < 0) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Server::_listen() {
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
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(8080);
	bindServerWithAddress();
	_listen();
}

Server::~Server() {
	for (size_t i = 0; i < clients.size(); i++) {
		close(clients[i].getFd());
	}

	close(serverSocketfd);
} 

void	uploadFile(Data& d) {
	std::string filename = d.getHead().at("filename");
	std::ofstream MyFile("/Users/amentag/Desktop/Webserv/uploads/" + filename);
	MyFile << d.getContent();
	MyFile.close();
}


const int&	Server::getServerSocketFd() const {
	return this->serverSocketfd;
}



int	Server::acceptNewConnection() {
	struct sockaddr_in	clientAddr;
	int					clientFd = -1;
	socklen_t			clientAddrLen;

	if ((clientFd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
		perror("Accept : ");
	} else {
		std::cout << "A new connection Accepted " << std::endl;

		Client client(clientFd);
		clients.push_back(client);
	}
	return clientFd;
}

void	Server::serverExists() {
	
	std::vector<t_server>	servers = config.servers;	

	for (int serverIndex = 0; serverIndex < (int)config.servers.size(); serverIndex++) {
		if (servers[serverIndex].serverName == currentRequest->getValueByKey(REQ_SERVER_NAME)
			&& atoi(currentRequest->getValueByKey(REQ_PORT).c_str()) == servers[serverIndex].port) {
			currentRequest->serverIndex = serverIndex;
			return ;
		}
	}
	currentRequest->serverIndex = -1;
	//error page : SERVER_NOT_FOUND
}

///END  OF client.cpp



static size_t	findCgiLocation(Request* request, t_location& location) {
	size_t index;

	index = location.path.find("*");
	if (index != std::string::npos) {
		std::string extention = &location.path[index + 1];
		size_t len = request->getValueByKey(REQ_PATH).length();
		if (extention != &request->getValueByKey(REQ_PATH)[len - extention.length()]) {
			return std::string::npos;
		}
	}
	return (index);
}

static std::string	getCgiPath(Request* request, t_location& location) {
	std::string lookFor = request->getValueByKey(REQ_PATH);
	size_t	last = lookFor.find_last_of('/');
	std::string	path;

	if (last != lookFor.npos) {
		lookFor.erase(last, -1);
		if (location.root[0] == '.') {// what if the path contains ./ ../ 
			path = (&location.root[1] != lookFor) ? location.root + request->getValueByKey(REQ_PATH) : "." + request->getValueByKey(REQ_PATH);
		} else {
			path = (location.root != lookFor) ? location.root + request->getValueByKey(REQ_PATH) : request->getValueByKey(REQ_PATH);
		}
	}
	return (path);
}

void	Server::locationExists() {
	std::vector<t_location>	locations = getServer().locations;
	size_t index;

	std::vector<std::string> locationPaths;
	for (size_t i = 0; i < locations.size(); i++) {
		index = findCgiLocation(currentRequest, locations[i]);
		if (index != std::string::npos) {
			std::string	cgiPath = getCgiPath(currentRequest, locations[i]);
			if (access(cgiPath.c_str(), F_OK) != -1) {
				currentRequest->locationIndex = i;
				config.servers[currentRequest->serverIndex].locations[i].isCgi = true;
				currentRequest->addToHead(REQ_PATH, cgiPath);
				return ;
			}
		}
		locationPaths.push_back(locations[i].path);
		config.servers[currentRequest->serverIndex].locations[i].isCgi = false;
	}
	
	std::string lookFor = currentRequest->getValueByKey(REQ_PATH);
	while (lookFor.size() > 0) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), lookFor);
		if (it != locationPaths.end()) {
			currentRequest->locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
		size_t	last = lookFor.find_last_of('/');
		if (last == lookFor.npos)
			break ;
		lookFor.erase(last, -1);
	}
	if (currentRequest->getValueByKey(REQ_PATH)[0] == '/' && lookFor.empty()) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), "/");
		if (it != locationPaths.end()) {
			currentRequest->locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
	}
}

const std::string&	Server::returnError(int status) {
	if (currentRequest->serverIndex >= 0) {
		t_server server = getServer();
		if (currentRequest->locationIndex >= 0) {
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
	response.setResponse(response.getHeader() + response.getBody());
	return (response.getResponse());
}

std::string	Server::matching() {
	serverExists(); // check error later!
	locationExists();
	if (currentRequest->locationIndex == -1) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
	t_location	location = config.servers[currentRequest->serverIndex].locations[currentRequest->locationIndex];
	std::string	pathToBeLookFor = currentRequest->getValueByKey(REQ_PATH);
	if (location.isCgi) {
		return (currentRequest->getValueByKey(REQ_PATH));
	}
	location.isCgi = false;
	pathToBeLookFor.erase(0, location.path.size());
	if (!location.root.empty()) {
		pathToBeLookFor.insert(0, location.root);
	} else {
		pathToBeLookFor.insert(0, config.servers[currentRequest->serverIndex].root);
	}
	return (pathToBeLookFor);
}

t_location&	Server::getLocation() {
	if (currentRequest->serverIndex < 0 || currentRequest->locationIndex < 0)
		throw std::out_of_range("getLocation()");
	return (config.servers[currentRequest->serverIndex].locations[currentRequest->locationIndex]);
}

t_server&	Server::getServer() {
	if (currentRequest->serverIndex < 0)
		throw std::out_of_range("getServer()");
	return (config.servers[currentRequest->serverIndex]);
}

static void	findAllowedMethod(std::string method, t_server& server, t_location& location) {
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
	std::string method = currentRequest->getValueByKey(REQ_METHOD);
	if (method != "GET" && method != "POST" && method != "DELETE") {
		throw std::runtime_error(returnError(NOT_IMPLEMENTED_STATUS));
	}
	try {
		findAllowedMethod(currentRequest->getValueByKey(REQ_METHOD), server, location);
	} catch (std::exception &ex) {
		throw std::runtime_error(returnError(METHOD_NOT_ALLOWED_STATUS));
	}
}

void	Server::locationRedirection() {
	t_location	location = getLocation();

	try {
		response.setBody(fileToString(location.redirectTo, NOT_FOUND_STATUS));
		response.setContentType(".html");
		response.setHeader(MOVED_PERMANENTLY_STATUS);
		response.setResponse(response.getHeader() + response.getBody());
	} catch(const std::exception& e) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Server::servFile(std::string& path) {
	try {
		response.setBody(fileToString(path, NOT_FOUND_STATUS));
		response.setHeader(200);
		response.setResponse(response.getHeader() + response.getBody());
	} catch(std::exception& ex) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Server::listDirectory(DIR *dir) {
	t_location location = getLocation();
	
	if (location.autoindex) {
		response.setContentType(".html");
		response.setBody(directory_listing(dir, currentRequest->getValueByKey(REQ_PATH)));
		response.setHeader(200);
		response.setResponse(response.getHeader() + response.getBody());
	} else if (!location.index.empty()) {
		initResponseClass(location.index);
		servFile(location.index);
	} else {
		throw std::runtime_error(returnError(FORBIDDEN_STATUS));
	}
	closedir(dir);
}

std::string convertToUpperCaseUnderscore(const std::string &input) {
    std::string result;
    for (size_t i = 0; i < input.size(); i++) {
        char c = input[i];
		if (std::isalpha(c)) {
            result += std::toupper(c);
        } else if (c == '-') {
            result += '_';
        }
		else
			result += c;
    }
    return result;
}

std::string	getKey(std::string str) {
	std::string DATA_FROM_CLIENT = "DATA_FROM_CLIENT";
	size_t index = str.find(DATA_FROM_CLIENT);
	if (index != std::string::npos) {
		return (str.erase(0, DATA_FROM_CLIENT.size()));
	}
	return (convertToUpperCaseUnderscore(str));
}

char	**getEnv(std::map<std::string, std::string> headMap) {
	char **env = new char*[headMap.size() + 1];
	int i = 0;

	std::map<std::string, std::string>::iterator it;
	for (it = headMap.begin(); it != headMap.end(); it++) {
		std::string key = getKey(it->first);
		env[i] = strdup((char *)(key + "=" + it->second).c_str());
		// std::cerr << env[i] << std::endl;
		i++;
	}
	env[i] = NULL;
	return (env);
}

static void assignHeadAndBody(std::string buffer, std::string& head, std::string& body) {
	size_t index = buffer.find("\r\n\r\n");

	if (index != std::string::npos) {
		head = buffer.substr(0, index + 4);
		body = buffer.substr(index + 4, buffer.length());
	}
}

void	Server::executeCgi(std::string path) {
	std::string head;
	std::string body;
	char		*program[3];
	pid_t pid;
	int			fd[2];
	char		buffer[MAX_LEN];
	t_location	location = config.servers[currentRequest->serverIndex].locations[currentRequest->locationIndex];
	program[0] = (char *)location.cgiExecutable.c_str();
	program[1] = (char *)path.c_str();
	program[2] = NULL;
	pipe(fd);
	// check if fork failed
	pid = fork(); 
	if (pid == -1)
		throw std::runtime_error("Fork failed");
	if (!pid) {
		dup2(fd[0], STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		close(fd[0]);
		execve(program[0], program, (char* const*)getEnv(currentRequest->getHead()));
		perror("Execve");
	}
	std::cout << "BODY REQUEST" << currentRequest->getValueByKey(REQ_QUERY_STRING) << std::endl;
	write(fd[1], currentRequest->getValueByKey(REQ_QUERY_STRING).c_str(), currentRequest->getValueByKey(REQ_QUERY_STRING).length());
	wait(0);
	bzero(buffer, MAX_LEN);

	read(fd[0], buffer, MAX_LEN);
	close(fd[0]);
	close(fd[1]);
	assignHeadAndBody(buffer, head, body);
	// std::cout << head << std::endl;
	// std::cout << body << std::endl;
	head.insert(0, "Content-length: " + to_string(body.length()) + "\r\n");
	head.insert(0, response.getStatusCode(200) + "\r\n");
	head.insert(0, response.getHttpVersion());
	response.setResponse(head + body);
}

void	Server::initResponseClass(std::string& path) {
	size_t dot = path.find_last_of('.');
	if (dot != path.npos) {
		if (path[path.length() - 1] == '/')
			path.erase(path.length() - 1);
		std::string extention = path.substr(dot, -1);
		response.setContentType(extention);
	} else {
		response.setContentType("");
	}
	response.setHttpVersion(currentRequest->getValueByKey(REQ_HTTP_VERSION));
	response.setStatusCode();
}

Request *Server::getRequestByFd(int clientFd) {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].getFd() == clientFd)
			return (clients[i].getRequest());
	}
	return (NULL);
}

void	Server::responseFunc(int fd) {
	try {
		currentRequest = getRequestByFd(fd);
		initResponseClass(currentRequest->getValueByKey(REQ_PATH));
		std::string path = matching();
		t_location location = getLocation();
		correctPath(path);
		DIR *dir = opendir(path.c_str());
		methodNotAllowed(); // should i check location errpage first when no method in the location?
		if (path == location.redirectFrom) {
			locationRedirection();
		} else if (dir) {
			listDirectory(dir);
		// } else if (location.isCgi && request->getValueByKey(REQ_METHOD) == "POST") {
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
	write(fd, response.getResponse().c_str(), response.getResponse().length());
	// std::cout << response.getResponse() << std::endl;
	currentRequest = NULL;
}

// void	Server::removeClients() {
// 	std::cout << "Hello " << std::endl;
// 	unsigned long current = clock();
// 	unsigned long diff;
// 	int				clientFd;

// 	for (size_t i = 0; i < clients.size(); i++) {
// 		diff = current - clients[i].getLastSeen();
// 		clientFd = clients[i].getFd();
// 		if (diff >= LAST_SEEN_TIMEOUT) {
// 			FD_CLR(clientFd, &current_sockets);
// 			close(clientFd);
// 		}
// 	}
// }

void	Server::dealWithClient(int clientIndex) {
	int fd = clients[clientIndex].getFd();
	try {
		clients[clientIndex].initRequest();
		currentRequest = clients[clientIndex].getRequest();
		if (currentRequest) {
			responseFunc(fd);
			delete (currentRequest);
			currentRequest = NULL;
			clients[clientIndex].setRequest(NULL);
		}
	} catch(const std::exception& e) {
		std::string error = e.what();
		if (error == "Bad Request")
			write(fd, "HTTP/1.1 400 BAD\r\n\r\n", 20);
		std::cout << PRINT_LINE_AND_FILE;
		std::cout << e.what() << std::endl;
	}
}


// void	Server::serve(fd_set& readyToReadFrom, fd_set& readyTowrite) {
// void	Server::serve() {
// 	fd_set readyToRead;

// 	FD_ZERO(&readyToRead);

// 	// removeClients(); [remove the clients that are less active]

// 	readyToRead = this->current_sockets;

// 	//read, write, error , timout
// 	if (select(FD_SETSIZE, &readyToRead, NULL, NULL, NULL) < 0) {
// 		perror("Select : ");
// 	}

// 	if (FD_ISSET(serverSocketfd, &readyToRead))
// 		acceptNewConnection();
// 	else
// 	{
// 		for (size_t i = 0; i < clients.size(); i++) {
// 			int fd = clients[i].getFd();
// 			if (FD_ISSET(fd, &readyToRead) && fd != serverSocketfd) {
// 				try {
// 					clients[i].initRequest();
// 					currentRequest = clients[i].getRequest();
// 					if (currentRequest) {
// 						responseFunc(fd);
// 						delete (currentRequest);
// 						currentRequest = NULL;
// 						clients[i].setRequest(NULL);
// 					}
// 				} catch(const std::exception& e) {
// 					std::string error = e.what();
// 					if (error == "Bad Request")
// 						write(fd, "HTTP/1.1 400 BAD\r\n\r\n", 20);
// 					std::cout << PRINT_LINE_AND_FILE;
// 					std::cout << e.what() << std::endl;
// 				}
// 			}
// 		}
// 	}
// }
