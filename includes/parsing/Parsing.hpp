#pragma once 

# include <sys/socket.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <signal.h>
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <sstream>
# include <macros.hpp>
# include <Request.hpp>

Request			*requestParse(std::string buffer);

typedef struct LocationDirectives {
	bool						autoindex;
	std::string					path;
	std::string					root;
	std::string					index;
	int							redirectionCode;
	// std::string					redirectFrom;
	std::string 				redirectTo;
	std::vector<std::string>	allowedMethods;
	std::map<int, std::string> 	errorPages;
	int					    	clientMaxBodySize;
	std::string                 cgiExecutable;
	bool                        isCgi;
}								t_location;

typedef struct ServerDirectives {
	int								port;
	int					    		clientMaxBodySize;
	std::map<int, std::string> 		errorPages;
	std::string						serverName;
	std::string						root;
	std::string						index;
	std::vector<std::string>        allowedMethods;
	std::vector<t_location>     	locations;
}									t_server;

typedef struct ConfigSettings {
	std::vector<t_server>	servers;
}							t_config;

/* extra functions */
std::string					        trim(const std::string& str);
bool						        bracketsBalance(const std::string& str);

/* Parsing Directives */
void                                getErrorPages(std::string& value, std::string& key, std::map<int, std::string>& errorPages);
std::vector<std::string>	        getAllowedMethods(std::string& value, std::string& key);
std::string					        getIndex(std::string& value, std::string& key);
std::string					        getRoot(std::string& value, std::string& key);
std::string					        getCgiExecutable(std::string& value, std::string& key);
std::string					        getServerName(std::string& value, std::string& key);
void						        getRedirect(std::string& value, std::string& key, int& redirectionCode, std::string& redirectTo);
bool						        getAutoIndex(std::string& value, std::string& key);
int							        getPort(std::string& value, std::string& key);
int							        getLimitClientBody(std::string& value, std::string& key);

/* Parse Locations */
void						        splitLocationBlocks(t_server& server, std::string res);

/* Parse Servers */
void						        splitServerBlocks(t_config& config, std::string res);

/* Parse Config File */
t_config					        parseConFile(const char* file);

/* Parse Request */
std::map<std::string, std::string>  fillContentTypeMap();
void	correctPath(std::string& path); // just for now
std::vector<std::string>	splitLine(std::string line, std::string delimiter);