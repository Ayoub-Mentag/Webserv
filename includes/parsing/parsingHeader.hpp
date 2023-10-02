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

/* ****** COLORS ****** */
# define RESET_COLOR "\033[0m"
# define RED          "\033[1;31m"
# define GREEN        "\033[1;32m"
# define YELLOW       "\033[1;33m"
# define ORANGE       "\033[1;34m"
# define MAGENTA      "\033[1;35m"
# define CYAN         "\033[1;36m"
# define WHITE        "\033[1;37m"

# define UNKNOWN_CHAR (char)200
# define DEFAULT_CONFIG_FILE "./configFiles/def.conf"

# define INVALID_ARGUMENT		RED "Error: " GREEN << key << " Invalid argument." << RESET_COLOR << "\n"
# define NO_VALUE				RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n"
# define PRINT_LINE_AND_FILE	YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR
# define INVALID_METHOD			RED "Error: " GREEN "Invalid method." << RESET_COLOR << "\n"
# define INVALID_LOC_DIRECTIVE	RED "Error: " GREEN "Invalid location Directive." << RESET_COLOR << "\n"
# define EXPECTED_SEM			RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n"
# define INVALID_DIRECTIVE		RED "Error: " GREEN "Invalid Directive." << RESET_COLOR << "\n"

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

/* extra functions */
std::string					trim(const std::string& str);
bool						bracketsBalance(const std::string& str);

/* Parsing Directives */
std::map<int, std::string>	getErrorPages(std::string& value, std::string& key);
std::vector<std::string>	getAllowedMethods(std::string& value, std::string& key);
std::string					getIndex(std::string& value, std::string& key);
std::string					getRoot(std::string& value, std::string& key);
std::string					getServerName(std::string& value, std::string& key);
void						getRedirect(std::string& value, std::string& key, std::string& redirectFrom, std::string& redirectTo);
bool						getAutoIndex(std::string& value, std::string& key);
int							getPort(std::string& value, std::string& key);
int							getLimitClientBody(std::string& value, std::string& key);

/* Parse Locations */
void						splitLocationBlocks(t_server& server, std::string res);

/* Parse Servers */
void						splitServerBlocks(t_config& config, std::string res);

/* Parse Config File */
t_config					parseConFile(const char* file);

/* Parse Request */
void                        requestParse(t_request& request, std::string buffer);
