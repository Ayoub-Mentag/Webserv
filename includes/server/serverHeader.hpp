#pragma once 

#include <parsingHeader.hpp>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LEN 3000
#define PORT 8080
#define BACKLOG 5

class Server {
	private:
		int			serverSocketfd;
		struct		sockaddr_in serverAddr;
		fd_set		current_sockets;
		t_config	config;

	private:
		std::string	matching(t_request &request);
		void		bindServerWithAddress();
		void		setPortOfListening();
		fd_set		getReadyFds();
		void		acceptNewConnection();
		void		response(int clientFd);
		void		sendFile(int fd, std::string fileName);

	public: 
		Server(t_config& config);
		~Server();
		void		serve();
};