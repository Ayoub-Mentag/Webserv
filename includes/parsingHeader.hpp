#pragma once

#define DEFAULT_CONFIG_FILE "./configFiles/def.conf"

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
#include <sstream>
#include <vector>
#include <map>

#define UNKNOWN_CHAR (char)200

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
    // int					    		clientMaxBodySize;
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



typedef struct Request {
    std::string	method;
    std::string	path;
    std::string	httpVersion;
    std::string	serverName;
    int			port;
}				t_request;

void 	requestParse(t_request& request, std::string buffer);