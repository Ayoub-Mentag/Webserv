
// static std::string	fileToString(std::string fileName, int status) {
// 	std::string result;
// 	std::string line;
// 	std::ifstream os;

// 	os.open(fileName);
// 	if (os.fail()) {
// 		switch (status) {
// 			case METHOD_NOT_ALLOWED_STATUS:
// 				throw std::runtime_error(DEFAULT_405_ERROR_PAGE);
// 			case NOT_FOUND_STATUS:
// 				throw std::runtime_error(DEFAULT_404_ERROR_PAGE);
// 			case FORBIDDEN_STATUS:
// 				throw std::runtime_error(DEFAULT_403_ERROR_PAGE);
// 			case BAD_REQUEST_STATUS:
// 				throw std::runtime_error(DEFAULT_400_ERROR_PAGE);
// 			case NOT_IMPLEMENTED_STATUS:
// 				throw std::runtime_error(DEFAULT_501_ERROR_PAGE);
// 			default:
// 				throw std::runtime_error(to_string(status) + " status code not handled");
// 		}
// 	}
// 	while (std::getline(os, line)) {
// 		result += line + "\n";
// 	}
// 	return (result);
// }

// void	correctPath(std::string& path) {
// 	DIR* dir = NULL;
// 	opendir(path.c_str());
// 	if (!dir) {
// 		if (path[path.length() - 1] == '/')
// 			path.erase(path.length() - 1);
// 		return ;
// 	}
// 	closedir(dir);
// }


// static void	findAllowedMethod(std::string& method, t_server& server, t_location& location) {
// 	bool		existInLocation = false;
// 	bool		existInServer = false;
// 	std::string	header;

// 	existInLocation = std::find(location.allowedMethods.begin(), location.allowedMethods.end(), method) != location.allowedMethods.end();
// 	if (existInLocation) {
// 		return ; // means that the method is allowed
// 	} else {
// 		if (location.allowedMethods.size() == 0) {
// 			existInServer = std::find(server.allowedMethods.begin(), server.allowedMethods.end(), method) != server.allowedMethods.end();
// 			if (existInServer)
// 				return ;
// 			throw std::runtime_error(server.errorPages[405]);
// 		}
// 		throw std::runtime_error(location.errorPages[405]);
// 	}
// }

#include <Server.hpp>
#include <Utils.hpp>

static std::string getDefaultErrorPage(int status) {
    switch (status) {
        case METHOD_NOT_ALLOWED_STATUS:
            return DEFAULT_405_ERROR_PAGE;
        case NOT_FOUND_STATUS:
            return DEFAULT_404_ERROR_PAGE;
        case FORBIDDEN_STATUS:
            return DEFAULT_403_ERROR_PAGE;
        case BAD_REQUEST_STATUS:
            return DEFAULT_400_ERROR_PAGE;
        case NOT_IMPLEMENTED_STATUS:
            return DEFAULT_501_ERROR_PAGE;
        default:
            return std::to_string(status) + " status code not handled";
    }
}

static std::string fileToString(const std::string& fileName, int status) {
    try {
        std::ifstream fileStream(fileName);
        if (!fileStream.is_open()) {
            throw std::runtime_error(getDefaultErrorPage(status));
        }

        std::string result;
        std::string line;
        while (std::getline(fileStream, line)) {
            result += line + '\n';
        }

        return result;
    } catch (const std::exception& e) {
        throw;
    }
}

void	Server::bindServerWithAddress() {
	int result = bind(this->serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Server::setPortOfListening() {
	if (listen(serverSocketfd, 5) == -1) {
		throw std::runtime_error("listen");
	}
}

Server::Server(t_config& config) : config(config) {
	if ((this->serverSocketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

/*
	Generates an HTML representation of the contents of a directory.
	Given a directory pointer `dir` and the `root` path, this function reads
	the directory entries and constructs an HTML list of links to each entry.
	If an entry is a directory, it adds a trailing slash to the link to indicate
	that it's a subdirectory.
*/
static std::string	directory_listing(DIR* dir, std::string root) {
	std::string response;
	response = "<html><body><ul>";

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		if (root[root.length() - 1] != '/') {
			root.insert(root.length(), "/");
		}
		response += "<li><a href=\"" + root + std::string(entry->d_name) +
					(entry->d_type == DT_DIR ? "/\">" : "\">") +
					std::string(entry->d_name) + "</a></li>";
	}
	response += "</ul></body></html>";
	return (response);
}

void	Server::initRequest(int clientFd) {
	/** @test we will work on some examples without getting the request from browser*/
	char		buffer[MAX_LEN];
	std::string	bufferLine;

	bzero(buffer, MAX_LEN);
	recv(clientFd, buffer, MAX_LEN, 0);
	bufferLine = buffer;
	// std::cerr << buffer;
	
	// print buffer after the checking
	request = requestParse(bufferLine);
	// i add this default valus in 24-10-2023 00:20
	request->serverIndex = -1; 
	request->locationIndex = -1;
}

void	Server::serverExists() {
	
	std::vector<t_server>	servers = config.servers;

	for (int serverIndex = 0; serverIndex < (int)config.servers.size(); serverIndex++) {
		if (servers[serverIndex].serverName == request->getHead()[REQ_SERVER_NAME]
			&& atoi(request->getHead()[REQ_PORT].c_str()) == servers[serverIndex].port) {
			request->serverIndex = serverIndex;
			return ;
		}
	}
	request->serverIndex = -1; // this line is usless cuz this var already contain this value as a def val
	//error page : SERVER_NOT_FOUND
}

/*
	This function checks if the specified location is intended for CGI handling.
	It examines the location's path and determines whether it contains a '*' character.
	If a '*' character is found, it indicates that the location should be used for CGI requests,
	and the function returns true. It also checks if the extension after '*' matches the request's path.
	If both conditions are met, it returns true; otherwise, it returns false.
*/
static bool	findCgiLocation(Request* request, t_location& location) {
	if (location.path[0] == '*') {
		std::string extention = &location.path[1];
		size_t len = request->getHead()[REQ_PATH].length();
		return (extention == &request->getHead()[REQ_PATH][len - extention.length()]);
	}
	return (false);
}

// i stil need to check this if it work properly 
static std::string	getCgiPath(Request* request, t_location& location) {
	std::string lookFor = request->getHead()[REQ_PATH];
	size_t	last = lookFor.find_last_of('/');
	std::string	path;

	if (last != lookFor.npos) {
		lookFor.erase(last, -1);
		if (location.root[0] == '.') {// what if the path contains ./ ../ 
			path = (&location.root[1] != lookFor) ? location.root + request->getHead()[REQ_PATH]
													: "." + request->getHead()[REQ_PATH];
		} else {
			path = (location.root != lookFor) ? location.root + request->getHead()[REQ_PATH]
												: request->getHead()[REQ_PATH];
		}
	}
	return (path);
}

void	Server::locationExists() {
	std::vector<t_location>		locations = getServer().locations;
	std::vector<std::string>	locationPaths;

	for (size_t i = 0; i < locations.size(); i++) {
		// here where the CGI location handled!
		if (findCgiLocation(request, locations[i])) {
			// is this the normal behavior?
			std::string	cgiPath = getCgiPath(request, locations[i]);
			if (access(cgiPath.c_str(), F_OK) != -1) {
				request->locationIndex = i;
				config.servers[request->serverIndex].locations[i].isCgi = true;
				request->getHead()[REQ_PATH] = cgiPath;
				// I have updated the request path that I received. Will this modification cause any issues?
				return ;
			}
		}
		locationPaths.push_back(locations[i].path);
		config.servers[request->serverIndex].locations[i].isCgi = false;
	}
	std::string lookFor = request->getHead()[REQ_PATH];
	while (lookFor.size() > 0) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), lookFor);
		if (it != locationPaths.end()) {
			request->locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
		size_t	last = lookFor.find_last_of('/');
		if (last == lookFor.npos)
			break ;
		lookFor.erase(last, -1);
	}
	if (request->getHead()[REQ_PATH][0] == '/' && lookFor.empty()) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), "/");
		if (it != locationPaths.end()) {
			request->locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
	}
}

/*
	This function generates an error response based on the provided HTTP status code.
	It checks if the server and location indices are set in the request object.
	If both indices are set, it attempts to retrieve an error page from the location's error pages.
	If the location's error page is not found, it falls back to using the server's error pages.
	If the server index is not set or any file retrieval fails, it sets the response body to an error message.
	The resulting response includes the HTTP status, content type, and the error page or error message.
	The response is then returned as a string.
*/
const std::string& Server::returnError(int status) {
    if (request->serverIndex < 0) {
        // Handle the case when the server index is not set.
        response.setContentType(".html");
        response.setHeader(status);
        response.setBody("Server index is not set.");
    } else {
        t_server server = getServer();

        if (request->locationIndex >= 0) {
            // Handle the case when the location index is set.
            t_location location = getLocation();
            try {
                response.setBody(fileToString(location.errorPages[status], status));
            } catch (const std::exception& e) {
                // Use the server's error pages if the location's error pages are not found.
                response.setBody(fileToString(server.errorPages[status], status));
            }
        } else {
            // Handle the case when the location index is not set.
            try {
                response.setBody(fileToString(server.errorPages[status], status));
            } catch (const std::exception& ex) {
                response.setBody(ex.what());
            }
        }
        response.setContentType(".html");
        response.setHeader(status);
    }
    return response.getResponse();
}

std::string	Server::matching() {
	serverExists(); // check error later!
	locationExists();
	if (request->locationIndex == -1) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
	const t_location&	location = getLocation();
	if (location.isCgi) {
		return (request->getHead()[REQ_PATH]);
	}
	// pathToBeLookFor.erase(0, location.path.size());
	// correctPath(location.root);
	std::string	pathToBeLookFor = request->getHead()[REQ_PATH];
	if (!location.root.empty()) {
		pathToBeLookFor.insert(0, location.root);
	} else {
		pathToBeLookFor.insert(0, config.servers[request->serverIndex].root);
	}
	return (pathToBeLookFor);
}

t_location&	Server::getLocation() {
	if (request->serverIndex < 0 || request->locationIndex < 0)
		throw std::out_of_range("getLocation()");
	return (config.servers[request->serverIndex].locations[request->locationIndex]);
}

t_server&	Server::getServer() {
	if (request->serverIndex < 0)
		throw std::out_of_range("getServer()");
	return (config.servers[request->serverIndex]);
}

/*
	Determines if a given HTTP request method is allowed based on the configuration settings.
	It checks whether the method is included in the allowed methods for the location.
	If the location-specific allowed methods are empty, it checks the server-level allowed methods.
	If the method is allowed in either the location or server configuration, it returns true.
	Otherwise, it returns false, indicating that the method is not allowed.
*/
static bool findAllowedMethod(const std::string& method, t_server& server, t_location& location) {
    bool inLocation = std::find(location.allowedMethods.begin(), location.allowedMethods.end(), method)
                    != location.allowedMethods.end();

    return (inLocation || (location.allowedMethods.empty() && std::find(server.allowedMethods.begin(),
            server.allowedMethods.end(), method) != server.allowedMethods.end()));
}

/*
	Checks if the HTTP request method is allowed based on server and location configuration.
	It retrieves the current server and location configurations.
	Then, it examines the HTTP request method to ensure it is either "GET," "POST," or "DELETE."
	If the method is not one of these allowed methods, it throws a "Not Implemented" error.
	Next, it checks whether the method is allowed in the server and location configurations.
	If the method is not allowed, it throws a "Method Not Allowed" error.
*/
void	Server::methodNotAllowed() {
	t_server			server = getServer();
	t_location			location = getLocation();
    const std::string&	method = request->getHead()[REQ_METHOD];

	if (method != "GET" && method != "POST" && method != "DELETE") {
		throw std::runtime_error(returnError(NOT_IMPLEMENTED_STATUS));
	} else if (findAllowedMethod(method, server, location) == false) {
		throw std::runtime_error(returnError(METHOD_NOT_ALLOWED_STATUS));
	}
}

// I should refactor this function to handle the format "return {code} {uri/path}".
void	Server::locationRedirection() {
	t_location	location = getLocation();

	try {
		response.setBody(fileToString(location.redirectTo, NOT_FOUND_STATUS));
		response.setContentType(".html");
		response.setHeader(MOVED_PERMANENTLY_STATUS);
	} catch(const std::exception& e) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Server::servFile(std::string& path, int status) {
	try {
		response.setBody(fileToString(path, NOT_FOUND_STATUS));
		response.setHeader(status);
	} catch(std::exception& ex) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Server::initResponseClass(std::string& path) {
	size_t len = (path[path.length() - 1] == '/') ? path.length() - 1 : path.length();
	size_t dot = path.find_last_of('.');

	(dot != path.npos) ? response.setContentType(path.substr(dot, len)) : response.setContentType("");
	response.setHttpVersion(request->getHead()[REQ_HTTP_VERSION]);
	response.setStatusCode();
}






void	Server::listDirectory(DIR *dir, std::string& path) {
	t_location location = getLocation();
	
	std::string indexFile = path + "/index.html";
	if (open(indexFile.c_str(), O_RDONLY) >= 0 && location.index.empty()) {
		try {
			response.setContentType(".html");
			servFile(indexFile, 200);
			// servFile(indexFile, NOT_MODIFIED_STATUS); // this is not working with 304 status code
			return ;
		} catch(const std::exception& e) {}
	}
	else if (!location.index.empty()) {
		if (location.index[0] == '/') {
			throw std::runtime_error(returnError(NOT_FOUND_STATUS));
		}
		if (location.index[0] != '/') {
			location.index.insert(0, "/");
		}
		indexFile = path + location.index;
		// www/index.html;
		try {
			response.setContentType(".html");
			servFile(indexFile, 200);
			// servFile(indexFile, NOT_MODIFIED_STATUS);
			return ;
		} catch(const std::exception& e) {}
	}
	if (location.autoindex) {
		response.setContentType(".html");
		response.setBody(directory_listing(dir, request->getHead()[REQ_PATH]));
		response.setHeader(200);
	} else {
		throw std::runtime_error(returnError(FORBIDDEN_STATUS));
	}
	closedir(dir);
}

void	Server::executeCgi(std::string path) {
	std::string body;
	char		*program[3];
	pid_t pid;
	int			fd[2];
	char		buffer[MAX_LEN];
	t_location	location = config.servers[request->serverIndex].locations[request->locationIndex];

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
}

void	Server::responseFunc(int clientFd) {
	try {
		std::cout << "req_path: " << request->getHead()[REQ_PATH] << std::endl;
		initResponseClass(request->getHead()[REQ_PATH]);
		std::string path = matching();
		t_location location = getLocation();
		// correctPath(path);
		DIR *dir = opendir(path.c_str());
		methodNotAllowed(); // should i check location errpage first when no method in the location?
		if (path == location.redirectFrom) {
			locationRedirection(); // recode this to handle (return -status code-)
		} else if (dir) {
			listDirectory(dir, path);
		} else if (location.isCgi) {
			executeCgi(path);
		} else {
			servFile(path, 200);
		}
	} catch (std::out_of_range &ofg) {
		std::cout << PRINT_LINE_AND_FILE;
		std::cout << ofg.what() << std::endl;
	} catch (std::exception &ex) {
		// just to catch the thrown error the response is already ready
	}
	// std::cout << response.getResponse();
	write(clientFd, response.getResponse().c_str(), response.getResponse().length());
	close(clientFd);
	FD_CLR(clientFd, &current_sockets);
}

void	Server::serve() {
	fd_set readySocket = getReadyFds();
	for (int fd = 0; fd < FD_SETSIZE; fd++) {
		if (FD_ISSET(fd, &readySocket)) {
			if (fd == this->serverSocketfd) {
				acceptNewConnection();
			} else {
				try {
					initRequest(fd);
					responseFunc(fd); 
				} catch(const std::exception& e) { // not handled !!!!!!!!
					std::cout << e.what() << std::endl;
				}
				delete (this->request);
				close(fd);
				FD_CLR(fd, &current_sockets);
			}
		}
	}
}
