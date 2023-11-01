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

class Server {
	private:
		int							serverSocketfd;
		struct						sockaddr_in serverAddr;
		fd_set						current_sockets;
		t_config					config;
		std::vector<Client>			clients;
		Response					response;
		Request				*currentRequest;

	private:
		std::string			matching();
		void				bindServerWithAddress();
		void				_listen();
		fd_set				getReadyFds();
		void				acceptNewConnection();
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

	public:
		Server();
		Server(t_config& config);
		~Server();

		void		serve();
	public: // responseClass fuctions
		void	initResponseClass(std::string& path);
		Request	*getRequestByFd(int clientFd);
};