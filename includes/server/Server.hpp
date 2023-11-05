#pragma once

# include <macros.hpp>
# include <exception>
# include <Parsing.hpp>
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
# include <Request.hpp>
# include <Response.hpp>
# include <macros.hpp>
# include <Client.hpp>

typedef struct	s_set_fd {
	fd_set	allSocketFds;
	fd_set	readyToReadFrom;
	fd_set	readyToWriteTo;
}	t_set_fd;

class Server {
	private:
		int					serverSocketfd;
		struct				sockaddr_in serverAddr;
		t_config			config;
		std::vector<Client>	clients;
		Response			response;
		Request				*currentRequest;

	private:
		std::string			matching();
		void				bindServerWithAddress();
		void				_listen();
		fd_set				getReadyFds();
		void				responseFunc(int clientFd);
		void				methodNotAllowed();
		void				locationRedirection();
		void				listDirectory(DIR *dir);
		void				servFile(std::string& src);
		void				serverExists();
		void				locationExists();
		void				executeCgi(std::string path);
		t_location&			getLocation();
		t_server&			getServer();
		const std::string&	returnError(int status);
		// void				deleteFile(std::string& path);
		void				removeClients(); 
	public:
		Server();
		void				dealWithClient(int clientIndex);
		int					acceptNewConnection();
		Server(t_config& config);
		~Server();
		const int&	getServerSocketFd() const;
		// void	serve(fd_set& readyToReadFrom, fd_set& readyTowrite);
		void	serve();

	public: // responseClass fuctions
		void	initResponseClass(std::string& path);
		Request	*getRequestByFd(int clientFd);
		const std::vector<Client>&	getClients() const;
};