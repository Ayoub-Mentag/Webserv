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
	std::string 				redirectTo;
	std::vector<std::string>	allowedMethods;
	std::map<int, std::string> 	errorPages;
	long long					clientMaxBodySize;
	std::string                 cgiExecutable;
	bool                        isCgi;
}								t_location;

typedef struct ServerDirectives {
	int								port;
	std::string						ipAddress;
	long long						clientMaxBodySize;
	std::map<int, std::string> 		errorPages;
	std::string						serverName;
	std::string						root;
	std::string						index;
	std::vector<std::string>        allowedMethods;
	std::vector<t_location>     	locations;
	int								redirectionCode;
	std::string 					redirectTo;
}									t_server;

typedef struct ConfigSettings {
	std::vector<t_server>	servers;
}							t_config;

/* extra functions */
std::string					trim(const std::string& str);
bool						bracketsBalance(const std::string& str);

/* Parsing Directives */
void                    	getErrorPages(std::string& value, const std::string& key, std::map<int, std::string>& errorPages);
std::vector<std::string>	getAllowedMethods(std::string& value, const std::string& key);
std::string					getIndex(std::string& value, const std::string& key);
std::string					getRoot(std::string& value, const std::string& key);
std::string					getCgiExecutable(std::string& value, const std::string& key);
std::string					getServerName(std::string& value, const std::string& key);
void						getRedirect(std::string& value, const std::string& key, int& redirectionCode, std::string& redirectTo);
bool						getAutoIndex(std::string& value, const std::string& key);
int							getPortAndIpAddress(std::string& value, const std::string& key, std::string& ipAddress);
long long					getLimitClientBody(std::string& value, const std::string& key);

/* Parse Locations */
void						parseLocationDirectives(std::string& key, std::string& value, t_location& location);


/* Parse Servers */
void						parseServerDirectives(std::string& key, std::string& value, t_server& server);
void						parseServerLines(std::string& line, t_server& server);
t_server					parseServerBlock(std::string line);

/* Parse Config File */
t_config					parseConFile(const char* file);
void						parseLocationLines(std::string& line, t_location& location);
t_location					parseLocationBlock(std::string line);

/* Parse Request */
std::vector<std::string>	splitLine(std::string line, std::string delimiter);