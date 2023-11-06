
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
#include <BoundaryRequest.hpp>
#include <PostRequest.hpp>
#include <DelGetRequest.hpp>
#include <Client.hpp>

//TODO: move the response code to the response


// static std::string getDefaultErrorPage(int status) {
//     switch (status) {
//         case METHOD_NOT_ALLOWED_STATUS:
//             return DEFAULT_405_ERROR_PAGE;
//         case NOT_FOUND_STATUS:
//             return DEFAULT_404_ERROR_PAGE;
//         case FORBIDDEN_STATUS:
//             return DEFAULT_403_ERROR_PAGE;
//         case BAD_REQUEST_STATUS:
//             return DEFAULT_400_ERROR_PAGE;
//         case NOT_IMPLEMENTED_STATUS:
//             return DEFAULT_501_ERROR_PAGE;
//         default:
//             return std::to_string(status) + " status code not handled";
//     }
// }

// static std::string fileToString(const std::string& fileName, int status) {
//     try {
//         std::ifstream fileStream(fileName);
//         if (!fileStream.is_open()) {
//             throw std::runtime_error(getDefaultErrorPage(status));
//         }

//         std::string result;
//         std::string line;
//         while (std::getline(fileStream, line)) {
//             result += line + '\n';
//         }

//         return (result);
//     } catch (const std::exception& e) {
//         throw;
//     }
// }

static std::string	fileToString(std::string fileName, int status) {
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
	os.close();
	return (result);
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

//TODO: Change t_config to t_server
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
	serverAddr.sin_addr.s_addr = inet_addr(config.servers[0].ipAddress.c_str());
	serverAddr.sin_port = htons(config.servers[0].port);
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

const std::vector<Client>&	Server::getClients() const {
	return this->clients;
}


int	Server::acceptNewConnection() {
	struct sockaddr_in	clientAddr;
	int					clientFd;
	socklen_t			clientAddrLen;

	if ((clientFd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
		perror("Accept : ");
	} else {
		// std::cout << "A new connection Accepted " << std::endl;

		Client client(clientFd);
		clients.push_back(client);
	}
	return clientFd;
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



void	Server::serverExists() {
	
	std::vector<t_server>	servers = config.servers;
	int			reqPort	= atoi(currentRequest->getValueByKey(REQ_PORT).c_str());
	std::string	reqHost = currentRequest->getValueByKey(REQ_SERVER_NAME);

	// i should check if the host is a server_name or an ip address check ./server/temp.cpp
	for (int serverIndex = 0; serverIndex < (int)config.servers.size(); serverIndex++) {
		std::string ipAdd = (servers[serverIndex].ipAddress == "127.0.0.1")
			? "localhost" : servers[serverIndex].ipAddress;

		if ((ipAdd == reqHost) // REQ_SERVER_NAME should become REQ_IP_ADD
			&& reqPort == servers[serverIndex].port) {
			currentRequest->serverIndex = serverIndex;
			return ;
		}
	}
	// in this case i should peek a default server if found else -1
	currentRequest->serverIndex = -1; // this line is usless cuz this var already contain this value as a def val
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
	std::vector<t_location>		locations = getServer().locations;
	std::vector<std::string>	locationPaths;

	for (size_t i = 0; i < locations.size(); i++) {
		// here where the CGI location handled!
		if (findCgiLocation(currentRequest, locations[i])) {
			// is this the normal behavior?
			std::string	cgiPath = getCgiPath(currentRequest, locations[i]);
			if (access(cgiPath.c_str(), F_OK) != -1) {
				currentRequest->locationIndex = i;
				config.servers[currentRequest->serverIndex].locations[i].isCgi = true;
				currentRequest->getHead()[REQ_PATH] = cgiPath;
				// I have updated the request path that I received. Will this modification cause any issues?
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

/*
	This function generates an error response based on the provided HTTP status code.
	It checks if the server and location indices are set in the request object.
	If both indices are set, it attempts to retrieve an error page from the location's error pages.
	If the location's error page is not found, it falls back to using the server's error pages.
	If the server index is not set or any file retrieval fails, it sets the response body to an error message.
	The resulting response includes the HTTP status, content type, and the error page or error message.
	The response is then returned as a string.
*/
const std::string&	Server::returnError(int status) {
    if (currentRequest->serverIndex < 0) {
        // Handle the case when the server index is not set.
        response.setContentType(".html");
        response.setHeader(status);
        response.setBody("Server index is not set.");
    } else {
        t_server server = getServer();

        if (currentRequest->locationIndex >= 0) {
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
	response.setResponse();
	return (response.getResponse());
}

std::string	Server::matching() {
	serverExists(); // check error later!
	locationExists();
	if (currentRequest->locationIndex == -1) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}

	const t_location&	location = getLocation();
	if (location.isCgi) {
		return (currentRequest->getValueByKey(REQ_PATH));
	}

	// pathToBeLookFor.erase(0, location.path.size());
	// correctPath(location.root);
	std::string	pathToBeLookFor = currentRequest->getValueByKey(REQ_PATH);
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
    const std::string&	method = currentRequest->getValueByKey(REQ_METHOD);

	if (method != "GET" && method != "POST" && method != "DELETE") {
		throw std::runtime_error(returnError(NOT_IMPLEMENTED_STATUS));
	} else if (findAllowedMethod(method, server, location) == false) {
		throw std::runtime_error(returnError(METHOD_NOT_ALLOWED_STATUS));
	}
}

// I should refactor this function to handle the format "return {code} {uri/path}".
void	Server::locationRedirection() {
	t_server	server = getServer();
	t_location	location = getLocation();
	std::string redirTo;
	int			redirCode;

	if (!server.redirectTo.empty()) {
		redirTo = server.redirectTo;
		redirCode = server.redirectionCode;
	} else {
		redirTo = location.redirectTo;
		redirCode = location.redirectionCode;
	}
	try {
		std::string	resp;
		response.setHeader(redirCode);
		resp = response.getHeader();
		size_t end = resp.find("\r\n\r\n");
		if (end != resp.npos) {
			resp.erase(end + 2, -1);
		}
		resp += "Location: " + redirTo + "\r\n\r\n";
		response.setResponse(resp);
	} catch(const std::exception& e) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Server::servFile(std::string& path, int status) {
	try {
		response.setBody(fileToString(path, NOT_FOUND_STATUS));
		response.setHeader(status);
		response.setResponse();
	} catch(std::exception& ex) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Server::initResponseClass(std::string& path) {
	size_t len = (path[path.length() - 1] == '/') ? path.length() - 1 : path.length();
	size_t dot = path.find_last_of('.');

	(dot != path.npos) ? response.setContentType(path.substr(dot, len)) : response.setContentType("");
	response.setHttpVersion(currentRequest->getHead()[REQ_HTTP_VERSION]);
	response.setStatusCode();
}

static bool	redirectDir(std::string& path, Response& response) {
	if (path[path.length() - 1] != '/') {
		std::string	resp;
		response.setHeader(MOVED_PERMANENTLY_STATUS);
		resp = response.getHeader();
		size_t end = resp.find("\r\n\r\n");
		if (end != resp.npos) {
			resp.erase(end + 2, -1);
		}
		resp += "Location: " + path + "/" + "\r\n\r\n";
		response.setResponse(resp);
		return (true);
	}
	return (false);
}

void	Server::listDirectory(DIR* dir, std::string& path) {
	t_location location = getLocation();

	// Redirect if the path lacks a trailing '/'
	if (redirectDir(path, response)) return ;
	// Check if the directory has an index file and the location's index is empty
	std::string indexFile = path + "/index.html";
	if (location.index.empty()) {
		try {
			response.setContentType(".html");
			servFile(indexFile, 200);
			// servFile(indexFile, NOT_MODIFIED_STATUS); // this is not working with 304 status code
			return ;
		} catch(const std::exception& e) {/* if not found keep going */}
	} else if (!location.index.empty()) {
		if (location.index[0] == '/') {
			throw std::runtime_error(returnError(NOT_FOUND_STATUS));
		}
		
		// i know that the path alawys will contain '/' at the end
		indexFile = path + location.index;
		try {
			response.setContentType(".html");
			servFile(indexFile, 200);
			// servFile(indexFile, NOT_MODIFIED_STATUS);
			return ;
		} catch(const std::exception& e) {/* if not found keep going */}
	}
	if (location.autoindex) {
		response.setContentType(".html");
		response.setBody(directory_listing(dir, currentRequest->getValueByKey(REQ_PATH)));
		response.setHeader(200);
		response.setResponse();
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

// static void assignHeadAndBody(std::string buffer, std::string& head, std::string& body) {
// 	size_t index = buffer.find("\r\n\r\n");

// 	if (index != std::string::npos) {
// 		head = buffer.substr(0, index + 4);
// 		body = buffer.substr(index + 4, buffer.length());
// 	}
// }

void	Server::executeCgi(std::string path) {
	std::string head;
	std::string body;
	char		*program[3];
	pid_t pid;
	int			fd[2];
	char		buffer[MAX_LEN];
	t_location	location = getLocation();

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
	body = buffer;
	body = body.substr(body.find("\r\n\r\n"), -1);

	response.setBody(body);
	response.setContentType(".html");
	response.setHeader(200);
	response.setResponse();
}

Request *Server::getRequestByFd(int clientFd) {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].getFd() == clientFd)
			return (clients[i].getRequest());
	}
	return (NULL);
}

void	Server::responseFunc(int clientFd) {
	try {
		// std::cout << "req_path: " << request->getHead()[REQ_PATH] << std::endl;
		initResponseClass(currentRequest->getHead()[REQ_PATH]);
		std::string path = matching();
		DIR *dir = opendir(path.c_str());
		methodNotAllowed(); // should i check location errpage first when no method in the location?
		if (!getServer().redirectTo.empty()
			|| !getLocation().redirectTo.empty()) {
			locationRedirection();
		} else if (dir) {
			listDirectory(dir, path);
		} else if (getLocation().isCgi) {
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
