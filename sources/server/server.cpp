#include <serverHeader.hpp>

void Server::sendFile(int fd, std::string fileName)
{
    std::ifstream	inFile;
	std::string		send;

	inFile.open(fileName);
	if (inFile.fail())
	{
		// throw std::runtime_error(fileName + " does not exist");
		sendFile(fd, "error.html");
	}	

	while (std::getline(inFile, send))
	{
		write(fd, send.c_str(), send.length());
		bzero((void *)send.c_str(), send.length());
	}
	inFile.close();
}

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
		throw std::runtime_error(strerror(errno));
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
	if ((clientFd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1)
	{
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
	std::string pathToBeLookFor = request.path;
	pathToBeLookFor.erase(0, locations[i].path.size());
	pathToBeLookFor.insert(0, locations[i].root);
	std::cout << "look = " << pathToBeLookFor << std::endl;
	return (pathToBeLookFor);
}

void Server::response(int clientFd)
{
	char		buffer[MAX_LEN];
	std::string	response;
	t_request	request;
	std::string	pathToBeLookFor;
	
	bzero(buffer, MAX_LEN);
	recv(clientFd, buffer, MAX_LEN, 0);
	std::cout << buffer;
	requestParse(request, buffer);
	try {
		pathToBeLookFor = matching(request);
		response = request.httpVersion + " 200 OK\r\nContent-type: text/html\r\nContent-length: 3000" + "\r\n\r\n";
		write(clientFd, response.c_str(), response.length());
		sendFile(clientFd, "." + pathToBeLookFor);
		close(clientFd);
		FD_CLR(clientFd, &current_sockets);
	} catch (std::exception &ex) {
		response = request.httpVersion + " 404 Not Found\r\nContent-type: text/html\r\nContent-length: 3000" + "\r\n\r\n";
		write(clientFd, response.c_str(), response.length());
		// sendFile(clientFd, location.errorFile);
	}
}

//accept , response
void Server::serve()
{
	fd_set readySocket = getReadyFds();
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (FD_ISSET(i, &readySocket))
		{
			if (i == this->serverSocketfd) {
				acceptNewConnection();
			}
			else {
				response(i);
			}
		}
	}
}
