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

class Server {
	private:
		int			serverSocketfd;
		struct		sockaddr_in serverAddr;
		fd_set		current_sockets;
		t_config	config;
		Request		*request;
		Response	response;

	private:
		std::string			matching();
		void				bindServerWithAddress();
		void				setPortOfListening();
		fd_set				getReadyFds();
		void				acceptNewConnection();
		void				responseFunc(int clientFd);
		void				methodNotAllowed();
		std::string			locationRedirection();
		std::string			listDirectory(DIR *dir);
		std::string			servFile(std::string& src);
		void				serverExists();
		void				locationExists();
		std::string			executeCgi(std::string path);
		t_location&			getLocation();
		t_server&			getServer();
		const std::string&	returnError(int status);
		void				deleteFile(std::string& path);
		void				initRequest(int clientFd);

	public:
		Server();
		Server(t_config& config);
		~Server();

		void		serve();

	public: // responseClass fuctions
		void	initResponseClass();
};