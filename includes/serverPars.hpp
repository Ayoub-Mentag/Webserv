#pragma once

#define DEFAULT_CONFIG_FILE "./configFiles/def.conf"

/* ****** COLORS ****** */
#define RED_TEXT "\033[1;31m"
#define GREEN_TEXT "\033[1;32m"
#define RESET_COLOR "\033[0m"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

void	parseConFile(const char* file);
void	usage(const char* programName);

typedef struct LocationDirectives {
    bool						autoindex;
    std::string					path;
    std::string					index;
    std::string					redirectFrom;
    std::string 				redirectTo;
    std::vector<std::string>	allowedMethods;
}								t_location;

typedef struct ServerDirectives {
    int								port;
    // int					    		clientMaxBodySize;
    std::map<int, std::string> 		errorPages;
    std::string						host;
    std::string						serverName;
    std::string						root;
    std::string						index;
    std::vector<t_location>     	locations;
}									t_server;

typedef struct ConfigSettings {
	std::vector<t_server>	servers;
}							t_config;
