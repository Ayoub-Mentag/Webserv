#include <parsingHeader.hpp>

#define MAX_LEN 1024
// add this macros to server.hpp later please
#define PORT 8080
#define BACKLOG 5
#define MAX_LEN 1024

// void	func() {
// 	system("leaks webservParsing");
// }

void sendFile(int fd, std::string fileName)
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
}

Server::~Server()
{
	close(serverSocketfd);
}
void Server::launchServer()
{
	bindServerWithAddress();
	setPortOfListening();
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
				exit(EXIT_FAILURE);
			}
			std::cout << "A new connection Accepted " << std::endl;
			FD_SET(clientFd, &current_sockets);
		}

		void 	Server::requestParse(t_request& request, std::string buffer) 
		{
			std::istringstream	iss(buffer);
			std::string			str;

			std::getline(iss, str);
			size_t first = str.find_first_of(' ');
			size_t last = str.find_last_of(' ');
			request.method = str.substr(0, first);
			request.httpVersion = str.substr(last + 1, -1);
			request.path = str.substr(first + 1, last - first - 1);
			std::getline(iss, str);
			last = str.find_last_of(':');
			request.port = std::atoi(str.substr(last + 1, -1).c_str());
			request.serverName = str.substr(6, last - 6);
			// while (std::getline(iss, str)) {
			// }
		}

		int Server::getLenOfMatching(std::string requestPath, std::string locationPath) {
			if (locationPath.size() > requestPath.size())
				return -1;
			int i = 0;
			while (i < (int)requestPath.size() & requestPath[i] == locationPath[i])
				i++;
			if (i == (int)locationPath.size() && (requestPath[i] == '/' || i == (int)requestPath.size()))
				return (i);
			return (-1);
		}

		std::string	Server::checkRequest(t_request &request)
		{
			int i = 0;
			int j = 0;
			int len = 0;
			int tmp;
			std::vector<t_location> locations;

			// exist on the server names in the config file
			// then nginx will route the request to the default server for this port

			// std::find()
			for (; i < (int)config.servers.size(); i++) {
				if (config.servers[i].serverName == request.serverName)
					break ;
			}
			if (i >= (int)config.servers.size())
				throw std::runtime_error("Server name not found");
			locations = config.servers[i].locations;
			
			i = -1;
			// matching the routes
			for (; j < (int)locations.size(); j++)
			{
				tmp = getLenOfMatching(request.path, locations[j].path);
				// std::cout << tmp << std::endl;
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
			std::cout << "look = " << locations[i].root << std::endl;
			return (pathToBeLookFor);
		}

		void Server::response(int clientFd)
		{
			char buffer[MAX_LEN];
			t_request request;
			std::string pathToBeLookFor;

			bzero(buffer, MAX_LEN);
			recv(clientFd, buffer, MAX_LEN, 0);

			requestParse(request, buffer);
			pathToBeLookFor = checkRequest(request);

			std::string responce = request.httpVersion + " 200 OK\r\nContent-type: text/html\r\nContent-length: 1024\r\n\r\n";
			write(clientFd, responce.c_str(), responce.length());
			sendFile(clientFd, "." + pathToBeLookFor);


			close(clientFd);
			FD_CLR(clientFd, &current_sockets);
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


