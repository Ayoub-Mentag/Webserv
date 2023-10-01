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


class Server {
	private :
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
		int serverSocketfd;
		struct sockaddr_in serverAddr;
		fd_set current_sockets;
		Server() 
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
		~Server()
		{
			close(serverSocketfd);
		}
		void launchServer()
		{
			bindServerWithAddress();
			setPortOfListening();
		}


		fd_set getReadyFds() {
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

		void acceptNewConnection()
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

		void 	requestParse(t_request& request, std::string buffer) 
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


		void response(int clientFd)
		{
			t_request r;
			char buffer[MAX_LEN];
			bzero(buffer, MAX_LEN);
			recv(clientFd, buffer, MAX_LEN, 0);
			try 
			{
				requestParse(r, buffer);

			} catch (std::exception &e)
			{
				std::cout << "Here my friend " << e.what() << std::endl;
			}
			std::string resp = r.httpVersion + " 200 OK\r\nContent-type: text/html\r\nContent-length: 1024\r\n\r\n";
			write(clientFd, resp.c_str(), resp.length());
			sendFile(clientFd, "." + r.path);
			close(clientFd);
			FD_CLR(clientFd, &current_sockets);
		}

		//accept , response
		void serve()
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

};





/*
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
		int serverSocketfd;
		struct sockaddr_in serverAddr;
		fd_set current_sockets;
		class Client 
		{
			public :
				t_request *request;
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen;
				int clientFd;
				Client() {
					clientFd = -1; 
				}
		};

		

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
						t_request r;
						char buffer[MAX_LEN];
						bzero(buffer, MAX_LEN);
						read(i, buffer, MAX_LEN);
						requestParse(r, buffer);
						response(i, r);
						close(i);
						FD_CLR(i, &current_sockets);
					}

				}
			}
		}

		void 	requestParse(t_request& request, std::string buffer) 
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

		void response(int fd, t_request &r)
		{
			std::cout << r.path << std::endl;
			std::string resp = r.httpVersion + " 200 OK\r\nontent-type: text/html\r\nContent-length: 1024\r\n\r\n";
			write(fd, resp.c_str(), resp.length());
			sendFile(fd, "." + r.path);
		}
};
*/


// int main()
// {
// 	Server s;

// 	s.launchServer();
// 	while (1)
// 	{
// 		s.serve();
// 	}
// 	return 0;
// }





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