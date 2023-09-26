
// /* ************************** Parse Location ****************************** */
// static void	fillLocationStruct(std::vector<std::string>& tokens) {
// 	t_location	location;
// 	for (size_t i = 0; i < tokens.size(); i++) {
// 		// parseDirectives();
// 		if (tokens[i].find("location") != tokens[i].npos)
// 				break ;
// 		std::istringstream	tokenStream(tokens[i]);
// 		std::string			key;
// 		std::string			value;

// 		std::getline(tokenStream, key, '=');
// 		std::getline(tokenStream, value);
// 		std::cerr << "location_key: " << key << "\n"; // for debuging  
// 	}
// 	return (location);
// }











#include "serverPars.hpp"

void	usage(const char* programName) {
	std::cerr << GREEN_TEXT "Usage: " RED_TEXT << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	exit(1);
}

// std::vector<std::string> split(const std::string& str) {
// 	std::vector<std::string> tokens;
// 	std::istringstream tokenStream(str);
// 	std::string token;

// 	std::cerr << str << "\n";
// 	while (tokenStream >> token) {
// 		tokens.push_back(token);
// 	}
// 	std::cout << token << " <=====\n";
// 	return tokens;
// }
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
// static void	parseDirectives();

static std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return ("");
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

static bool	bracketsBalance(const std::string& str) {
	std::stack<char> stack;
	char c;
	for (size_t i = 0; i < str.length(); i++) {
		c = str[i];
		if (c == '{' || c == '[') {
			stack.push(c);
		} else if (c == '}' && (!stack.empty() && stack.top() == '{')) {
			stack.pop();
		} else if (c == ']' && (!stack.empty() && stack.top() == '[')) {
			stack.pop();
		}
    }
	return (stack.empty());
}























static void	parseLocationDirectives(std::string& key, std::string& value, t_location& location) {
	if (key == "allowed_methods") {
		// location.allowedMethods = value;
	} else if (key == "index") {
		location.index = value;
	} else if (key == "autoindex") {
		location.autoindex = atoi(value.c_str());
	} else if (key == "redirect") {
		// location.redirect = atoi(value.c_str());
	} else if (key == "root") {
		location.root = value;;
	} else {
		std::cerr << RED_TEXT "Error: " GREEN_TEXT "Invalid location Directive." << RESET_COLOR << "\n";
	}
}

static void	fillLocationStruct(t_location& location, std::vector<std::string>& tokens) {

	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i].find("location") != tokens[i].npos)
				break ;
		std::istringstream	tokenStream(tokens[i]);
		std::string			key;
		std::string			value;

		std::getline(tokenStream, key, '=');
		std::getline(tokenStream, value);
		key = trim(key);
		value = trim(value);
		parseLocationDirectives(key, value, location); 
	}
}

static t_location*	parseLocationBlock(std::string res) {
	std::vector<std::string>	locationTokens;
	t_location					*location = new t_location;

	size_t findBrack = res.find("{");
	std::string					token;
	if (findBrack != res.npos) {
		std::string path = res.substr(0, findBrack);
		location->path = path;
		res = res.substr(findBrack + 1, -1);
		// location->path = path;  
	}
	std::istringstream			tokenStream(res);
	while (std::getline(tokenStream, token, ';')) {
		if (token == "}" || token == "}}") {
			continue ;
		}
		locationTokens.push_back(token);  
	}
	fillLocationStruct(*location, locationTokens);
	return (location);
}

static void	splitLocationBlocks(t_server& server, std::string res) {
	while (res.size()) {
		std::string	serverBlock;
		size_t fserv = res.find("location");
		if (fserv != std::string::npos) {
			res = res.substr(9, std::string::npos);
			fserv = res.find("location");
			if (fserv != std::string::npos) {
				t_location *tmp = parseLocationBlock(res.substr(0, fserv));
				server.locations.push_back(*tmp);
				delete tmp;
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			t_location *tmp = parseLocationBlock(res);
			server.locations.push_back(*tmp);
			delete tmp;
			break ;
		}
	}
}

// /* **************************** Parse Server ****************************** */

static void	parseServerDirectives(std::string& key, std::string& value, t_server& server) {
	if (key == "server_name") {
		server.serverName = value;
	} else if (key == "listen") {
		server.port = atoi(value.c_str());
	} else if (key == "root") {
		server.root = value;
	} else if (key == "index") {
		server.index = value;
	} else if (key == "error_page") {
		// server.errorPages
	} else {
		std::cerr << RED_TEXT "Error: " GREEN_TEXT "Invalid Directive." << RESET_COLOR << "\n";
	}
}

static void	fillServerStruct(t_server& server, std::vector<std::string>& tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		// parseDirectives();
		if (tokens[i].find("location") != tokens[i].npos)
				break ;
		std::istringstream tokenStream(tokens[i]);
		std::string key;
		std::string value;
		std::getline(tokenStream, key, '=');
		std::getline(tokenStream, value);
		key = trim(key);
		value = trim(value);
		parseServerDirectives(key, value, server);  
	}
}

static t_server* parseServerBlock(std::string res) {
	std::vector<std::string>	serverTokens;
	t_server					*server = new t_server;

	size_t findBrack = res.find("{");
	if (findBrack != res.npos) {
		res = res.substr(findBrack + 1, -1);
	}
	std::istringstream			tokenStream(res);
	std::string					token;
	while (std::getline(tokenStream, token, ';')) {
		if (token.find("location") != token.npos) {
			break ;
		}
		serverTokens.push_back(token);  
	}
	fillServerStruct(*server, serverTokens);
	splitLocationBlocks(*server, res.substr(res.find("location"), -1));
	return (server);
}

static void	splitServerBlocks(t_config& config, std::string res) {
	while (res.size()) {
		std::string	serverBlock;
		size_t fserv = res.find("Server");
		if (fserv != std::string::npos) {
			res = res.substr(6, std::string::npos);
			fserv = res.find("Server");
			if (fserv != std::string::npos) {
				t_server *tmp = parseServerBlock(res.substr(0, fserv));
				config.servers.push_back(*tmp);
				delete tmp;
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			t_server *tmp = parseServerBlock(res);
			config.servers.push_back(*tmp);
			delete tmp;
			break ;
		}
	}
}

void	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	*config = new t_config;
	std::string	line;
	std::string res;

	std::ifstream configFile(file);
	if (!configFile.is_open()) {
		std::cerr << "Failed to open the configuration file." << std::endl;
		exit(1);
	}

	while (std::getline(configFile, line)) {
		// ignore empty lines and comments
		size_t commentPos = line.find("#");
		if (commentPos != std::string::npos) {
			line.erase(commentPos, line.length());
		}
		line = trim(line);
		if (line.empty()) {
			continue ;
		}
		res += line;
	}
	if (!bracketsBalance(res)) {
		std::cerr << "Error: Unclosed bracket in configuration file." << std::endl;
	}

	splitServerBlocks(*config, res);

	for (size_t i = 0; i < config->servers.size(); i++) {
		std::cerr << "host:" << config->servers[i].host  << "\n";
		std::cerr << "index:" << config->servers[i].index  << "\n";
		std::cerr << "port:" << config->servers[i].port  << "\n";
		std::cerr << "root:" << config->servers[i].root  << "\n";
		std::cerr << "servername:" << config->servers[i].serverName << "\n";
		for (size_t j = 0; j < config->servers[i].locations.size(); j++) {
			// std::cerr << j << "	location_allowedMethods			" << config->servers[i].locations[j].allowedMethods  << "\n";
			std::cerr << j << "	location_autoindex			" << config->servers[i].locations[j].autoindex  << "\n";
			std::cerr << j << "	location_index			" << config->servers[i].locations[j].index  << "\n";
			std::cerr << j << "	location_path			" << config->servers[i].locations[j].path << "\n";
			std::cerr << j << "	location_redirectFrom			" << config->servers[i].locations[j].redirectFrom << "\n";
			std::cerr << j << "	location_redirectTo			" << config->servers[i].locations[j].redirectTo << "\n";
		}
	}
	
	delete config;
	configFile.close();    
}
