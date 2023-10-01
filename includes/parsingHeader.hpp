#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

/* ****** COLORS ****** */
#define RESET_COLOR "\033[0m"
#define RED          "\033[1;31m"
#define GREEN        "\033[1;32m"
#define YELLOW       "\033[1;33m"
#define ORANGE       "\033[1;34m"
#define MAGENTA      "\033[1;35m"
#define CYAN         "\033[1;36m"
#define WHITE        "\033[1;37m"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#define UNKNOWN_CHAR (char)200
#define DEFAULT_CONFIG_FILE "./configFiles/def.conf"

typedef struct Request {
    std::string method;
    std::string path;
    std::string httpVersion;
    std::string serverName;
    int         port;
} t_request;

typedef struct LocationDirectives {
    bool						autoindex;
    std::string					path;
    std::string					root;
    std::string					index;
    std::string					redirectFrom;
    std::string 				redirectTo;
    std::vector<std::string>	allowedMethods;
}								t_location;

typedef struct ServerDirectives {
    int								port;
    int					    		clientMaxBodySize;
    std::map<int, std::string> 		errorPages;
    std::string						serverName;
    std::string						root;
    std::string						index;
    std::vector<t_location>     	locations;
}									t_server;

typedef struct ConfigSettings {
	std::vector<t_server>	servers;
}							t_config;

t_config	parseConFile(const char* file);
void    	usage(const char* programName);

class Server {
	private :
		int serverSocketfd;
		struct sockaddr_in serverAddr;
		fd_set current_sockets;
		t_config	config;

	private :
		void bindServerWithAddress();
		void setPortOfListening();

	public : 
		Server(t_config& config);
		~Server();
		void launchServer();
		fd_set getReadyFds();
		void acceptNewConnection();
		void 	requestParse(t_request& request, std::string buffer);
		int getLenOfMatching(std::string requestPath, std::string locationPath);
		std::string	checkRequest(t_request &request);
		void response(int clientFd);
		//accept , response
		void serve();

};