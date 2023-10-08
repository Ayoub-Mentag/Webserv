#pragma once 

# include <exception>
# include <parsingHeader.hpp>
# include <sys/socket.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <string.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <signal.h>
# include <stdlib.h>
# include <dirent.h>

# define MAX_LEN 650
# define PORT 8080
# define BACKLOG 5

# define DEFAULT_400_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>400 Bad Request</h1></body></html>"
# define DEFAULT_403_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>403 Forbidden</h1></body></html>"
# define DEFAULT_404_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>404 Not Found</h1></body></html>"
# define DEFAULT_405_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>405 Method Not Allowed</h1></body></html>"
# define DEFAULT_501_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>501 Not Implemented</h1></body></html>"

# define BAD_REQUEST_STATUS			400
# define NOT_FOUND_STATUS 			404
# define METHOD_NOT_ALLOWED_STATUS 	405
# define FORBIDDEN_STATUS			403
# define NOT_IMPLEMENTED			501

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
		void		response(int clientFd, std::string src, t_request& request);
		t_request	getRequest(int clientFd);
		void		methodNotAllowed(t_request& request);
		std::string	locationRedirection(t_request& request);
		std::string	listDirectory(t_request& request, DIR *dir);
		std::string	servFile(std::string& src, t_request& request);
		void		serverExists(t_request& request);
		void		locationExists(t_request& request);
		std::string	executeCgi(std::string path, t_request request);

	public: 
		Server(t_config& config);
		~Server();
		void		serve();
		t_location&	getLocation(int serverIndex, int locationIndex);
		t_server&	getServer(int serverIndex);
};
