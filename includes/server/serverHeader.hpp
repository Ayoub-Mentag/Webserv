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

# define MAX_LEN	650
# define PORT		8080
# define BACKLOG	5

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

typedef class Response {
	private:
		std::string					mimeType; // content-type
		std::string					httpVersion;
		std::map<int, std::string>	statusCode;
		std::string					header;
		std::string					body;
		std::string					response;
	public:
		Response();
		~Response();

		const std::string&			getHeader();
		const std::string&			getBody();
		const std::string& 			getStatusCode(int status);
		const std::string&			getHttpVersion();
		const std::string&			getContentType();

		void						setHeader();
		void						setBody();
		void			 			setStatusCode();
		void						setHttpVersion(const std::string&);
		void						setContentType(const std::string& contentType);
		// const std::string&			getResponse();
}               	                t_response;

class Server {
	private:
		int			serverSocketfd;
		struct		sockaddr_in serverAddr;
		fd_set		current_sockets;
		t_config	config;
		t_response	response;
		t_request	request;

	private:
		std::string	matching();
		void		bindServerWithAddress();
		void		setPortOfListening();
		fd_set		getReadyFds();
		void		acceptNewConnection();
		void		responseFunc(int clientFd, std::string src);
		void		initRequest(int clientFd);
		void		methodNotAllowed();
		std::string	locationRedirection();
		std::string	listDirectory(DIR *dir);
		std::string	servFile(std::string& src);
		void		serverExists();
		void		locationExists();
		std::string	executeCgi(std::string path);
		t_location&	getLocation();
		t_server&	getServer();

	public:
		Server();
		Server(t_config& config);
		~Server();

		void		serve();
};
