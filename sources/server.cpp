#include <server.hpp>
// add this macros to server.hpp later please
#define PORT 8085
#define BACKLOG 5
#define MAX_LEN 1024

void sendFile(int fd, std::string fileName)
{
    std::ifstream	inFile;
	std::string		send;

	inFile.open(fileName);
	if (inFile.fail())
		throw std::runtime_error(fileName + " does not exist");

	while (std::getline(inFile, send))
	{
		write(fd, send.c_str(), send.length());
		bzero((void *)send.c_str(), send.length());
	}
	inFile.close();
}

std::string readFromFd(int fd)
{
	char buffer[MAX_LEN];
	std::string str;
	int n;

	// read(fd, buffer, sizeof(buffer));
	// str.append(buffer);
	// read(fd, buffer, sizeof(buffer));
	while ((n = read(fd, buffer, sizeof(buffer))) > 0)
	{
		std::cout << " ------ " << n << std::endl;
		str.append(buffer);
		bzero(buffer, MAX_LEN);
		n = 0;
	}
	str.append(buffer);
	return str;
}

class Request 
{
	public: 
		std::string method;
		int port;
		std::string path;
};

class Socket {
	private:
		void bindServerWithAddress()
		{
			int result = bind(this->serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
			if (result == -1)
			{
				throw std::runtime_error(strerror(errno));
			}
		}
		
		void setPortOfListening()
		{
			if (listen(serverSocketfd, 5) == -1)
			{
				throw std::runtime_error(strerror(errno));
			}
		}

	public : 
		class Client 
		{
			public :
				Request *request;
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen;
				int clientFd;
				Client() {
					clientFd = -1; 
				}
		};

		int serverSocketfd;
		struct sockaddr_in serverAddr;
		fd_set current_sockets;

		Socket() 
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

		~Socket()
		{
			close(serverSocketfd);
		}

		void launchServer()
		{
			bindServerWithAddress();
			setPortOfListening();
		}

		void serveNewConnection()
		{
			fd_set ready_socket;

			FD_ZERO(&ready_socket);
			ready_socket = current_sockets;
			//read, write, error , timout
			if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL) < 0)
			{
				perror("Select : ");
				exit(1);
			}

			for (int i =0; i < FD_SETSIZE; i++)
			{

				if (FD_ISSET(i, &ready_socket))
				{
					Client client;
					if (i == serverSocketfd)
					{
						// a new connection
						if ((client.clientFd = accept(serverSocketfd, (struct sockaddr*)&client.clientAddr, &client.clientAddrLen)) == -1)
						{
							perror("Accept : ");
							exit(EXIT_FAILURE);
						}
						std::cout << "A new connection Accepted " << std::endl;
						FD_SET(client.clientFd, &current_sockets);
					}

					else
					{
						char buffer[MAX_LEN];
						std::string request;
						bzero(buffer, MAX_LEN);
						read(i, buffer, MAX_LEN);
						request = buffer;
						// setRequest(client, request);
						std::cout << buffer << std::endl;
						sayHello(i);
						close(i);
						FD_CLR(i, &current_sockets);
					}

				}
			}
		}

		void setRequest(Client &client, std::string request)
		{
			int j = 0;
			std::string path = "";
			int index = request.find("GET");
			client.request->method = "GET";
			for (int i = index; i < (int)request.length(); i++)
			{
				path[i] += request[i];
			}
			client.request->path = path;
			client.request->port = PORT;

			std::cout << "Method " << client.request->method << " ; path " << client.request->path << " ; port " << client.request->method << std::endl;
		}


		void sayHello(int fd)
		{
			write(fd, "HTTP/1.1 200 OK\r\n" , 17);
			write(fd, "Content-Type: text/html\r\n" , 25);
			write(fd, "Content-Length: 1024\r\n", 22);
			write(fd, "\r\n" , 2);
			
			sendFile(fd, "index.html");
			// write(fd, "Hello txt\r\n", 11);
		}


};


int main()
{
	Socket s;

	s.launchServer();
	while (1)
	{
		s.serveNewConnection();
	}
	return 0;
}





/*
	int main()
	{
		int serverSocketfd, clientSocketfd;
		struct sockaddr_in serverAddr, clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		serverSocketfd = socket(AF_INET, SOCK_STREAM, 0);
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(PORT);


		int bind_result;

		bind_result = bind(serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
		if (bind_result == -1) {
			return (perror("Bind"), 1);
		}
		std::cout << "Bounded successfully .." << std::endl;
		if (listen(serverSocketfd, 5) == -1)
			return (perror("Listen : "), 1);
		std::cout << "Listned successfully .." << std::endl;
		if ((clientSocketfd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1)
			return (perror("Accept : "), 1);
		std::cout << "A client has been entered" << std::endl;

		char buffer[MAX_LEN];
		bzero(buffer, MAX_LEN);
		std::string request = readFromFd(clientSocketfd);
		std::cout << request;
		parseRequest(request);
		write(clientSocketfd, "HTTP/1.1 200 OK\r\n\r\n", 19);
		sendFile(clientSocketfd , "index.html");
		close(clientSocketfd);





		if (!strncmp(received, "GET / HTTP/1.1", 14))
		{
			write(clientSocketfd, "HTTP/1.1 200 OK\r\n\r\n", 19);
			sendFile(clientSocketfd, "");
		}
		else if (!strncmp(received, "GET /test/test.html HTTP/1.1", 28))
		{
			printf("hello\n");
			write(clientSocketfd, "HTTP/1.1 200 OK\r\n\r\n", 19);
			sendFile(clientSocketfd, "test/test.html");
		}
		else
			write(clientSocketfd, "404 NOT FOUND :(\n", 17);
		close(clientSocketfd);
		close(serverSocketfd);
		exit(0);
	}
*/