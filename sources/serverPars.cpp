#include "serverPars.hpp"

void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
	exit(1);
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

static std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return ("");
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

// static bool	bracketsBalance(const std::string& str) {
// 	std::stack<char> stack;
// 	char c;
// 	for (size_t i = 0; i < str.length(); i++) {
// 		c = str[i];
// 		if (c == '{' || c == '[') {
// 			stack.push(c);
// 		} else if (c == '}' && (!stack.empty() && stack.top() == '{')) {
// 			stack.pop();
// 		} else if (c == ']' && (!stack.empty() && stack.top() == '[')) {
// 			stack.pop();
// 		}
//     }
// 	return (stack.empty());
// }

static bool bracketsBalance(const std::string& str) {
    std::stack<char> stack;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (c == '{' || c == '[') {
            stack.push(c);
        } else if (c == '}' || c == ']') {
            if (stack.empty()) {
                return false; // Closing bracket with no matching opening bracket
            }
            if ((c == '}' &&  stack.top() == '{') || (c == ']' &&  stack.top() == '[')) {
                stack.pop();
            } else {
                return false; // Mismatched brackets
            }
        }
    }
    return stack.empty(); // Check for extra opening brackets
}



/* ************************** Parse Directives ****************************** */
std::vector<std::string>		getAllowedMethods(std::string& value, std::string& key) {
	std::string					token;
	std::vector<std::string>	allowedMethods;
		
	if (value.empty() || value == ";") {
		std::cerr << RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	std::istringstream			toSplit(value);
	while (std::getline(toSplit, token, ' ')) {
		if (token == "") continue;
		if (token != "GET" && token != "POST" && token != "DELETE") {
			std::cerr << RED "Error: " GREEN "Invalid method." << RESET_COLOR << "\n";
			std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
			exit(1);
		}
		allowedMethods.push_back(token);
	}
	return (allowedMethods);
}

std::string	getIndex(std::string& value, std::string& key) {
	if (value.empty() || value == ";") {
		std::cerr << RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	return (value);
}

bool	getAutoIndex(std::string& value, std::string& key) {
	if (value.empty() || value == ";") {
		std::cerr << RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	value = trim(value);
	if (value == "0" || value == "false" || value == "FALSE") {
		return (false);
	} else if (value == "1" || value == "true" || value == "TRUE") {
		return (true);
	} else {
		std::cerr << RED "Error: " GREEN << key << " Invalid argument." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
}

void	getRedirect(std::string& value, std::string& key, std::string& redirectFrom, std::string& redirectTo) {
	if (value.empty() || value == ";") {
		std::cerr << RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	// size_t first = value.find_first_not_of(" \t\n\r");
	// if (first != std::string::npos)
		// std::cout << "-" << value.substr(0, first) << "-\n";
		std::cout << "-" << value << "-\n";
		


}

/* ************************** Parse Location ****************************** */
static void	parseLocationDirectives(std::string& key, std::string value, t_location& location) {
	if (value[value.length() - 1] != ';') {
		std::cerr << RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	size_t last = value.find_last_not_of(';');
	if (last != value.npos && value[last + 1] == ';')
		value = value.substr(0, last + 1);
	std::cerr << "location_value:" << value << "\n";
	if (key == "allowed_methods") {
		location.allowedMethods = getAllowedMethods(value, key);
	} else if (key == "index") {
		location.index = getIndex(value, key);
	} else if (key == "autoindex") {
		location.autoindex = getAutoIndex(value, key);
	} else if (key == "redirect") {
		getRedirect(value, key, location.redirectFrom, location.redirectTo);
	} else if (key == "root") {
		// location.root = value;;
	} else {
		std::cerr << RED "Error: " GREEN "Invalid location Directive." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
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

static t_location	parseLocationBlock(std::string res) {
	std::vector<std::string>	locationTokens;
	t_location					location;
	std::string					token;

	size_t findBrack = res.find("{");
	if (findBrack != res.npos) {
		location.path = res.substr(0, findBrack);
		res = res.substr(findBrack + 1, -1); 
	}
	if (res[0] == UNKNOWN_CHAR)
		res[0] = ' ';
	std::istringstream			tokenStream(res);
	while (std::getline(tokenStream, token, UNKNOWN_CHAR)) {
		if (token[0] == '}' && token[token.length() - 1] == '}') {
			continue ;
		}
		locationTokens.push_back(token);
	}
	fillLocationStruct(location, locationTokens);
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
				server.locations.push_back(parseLocationBlock(res.substr(0, fserv)));
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			server.locations.push_back(parseLocationBlock(res));
			break ;
		}
	}
}

// /* **************************** Parse Server ****************************** */
static void	parseServerDirectives(std::string& key, std::string& value, t_server& server) {
	if (value[value.length() - 1] != ';') {
		std::cerr << RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	std::cerr << "server_value:" << value << "\n";
	if (key == "server_name") {
		server.serverName = value;
	} else if (key == "listen") {
		server.port = atoi(value.c_str());
	} else if (key == "root") {
		server.root = value;
	} else if (key == "index") {
		server.index = value;
	} else if (key == "error_page") {
	
	} else {
		std::cerr << RED "Error: " GREEN "Invalid Directive." << RESET_COLOR << "\n";
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
}

static void	fillServerStruct(t_server& server, std::vector<std::string>& tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
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

static t_server	parseServerBlock(std::string res) {
	std::vector<std::string>	serverTokens;
	t_server					server;

	size_t findBrack = res.find("{");
	if (findBrack != res.npos) {
		res = res.substr(findBrack + 1, -1);
	}
	if (res[0] == UNKNOWN_CHAR)
		res[0] = ' ';
	std::istringstream			tokenStream(res);
	std::string					token;
	while (std::getline(tokenStream, token, UNKNOWN_CHAR)) {
		if (token.find("location") != token.npos) {
			break ;
		}
		if (token[0] == '}' && token[token.length() - 1] == '}') {
			continue ;
		}
		serverTokens.push_back(token);  
	}
	fillServerStruct(server, serverTokens);
	size_t	floc = res.find("location");
	if (floc != res.npos)
		splitLocationBlocks(server, res.substr(floc, -1));
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
				config.servers.push_back(parseServerBlock(res.substr(0, fserv)));
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			config.servers.push_back(parseServerBlock(res));
			break ;
		}
	}
}

t_config*	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	*config = new t_config;
	std::string	line;
	std::string res;

	std::ifstream configFile(file);
	if (!configFile.is_open()) {
		std::cerr << RED "Error:" GREEN "Failed to open the configuration file." RESET_COLOR << std::endl;
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
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
		res += line + UNKNOWN_CHAR;
	}
	if (!bracketsBalance(res)) {
		std::cerr << RED "Error: " GREEN "Unclosed bracket in configuration file." RESET_COLOR<< std::endl;
		std::cerr << YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR;
		exit(1);
	}
	// std::cout << "res:" << res;
	splitServerBlocks(*config, res);

	// for (size_t i = 0; i < config->servers.size(); i++) {
	// 	std::cerr << "host:" << config->servers[i].host  << "\n";
	// 	std::cerr << "index:" << config->servers[i].index  << "\n";
	// 	std::cerr << "port:" << config->servers[i].port  << "\n";
	// 	std::cerr << "root:" << config->servers[i].root  << "\n";
	// 	std::cerr << "servername:" << config->servers[i].serverName << "\n";
	// 	for (size_t j = 0; j < config->servers[i].locations.size(); j++) {
	// 		// std::cerr << j << "	location_allowedMethods			" << config->servers[i].locations[j].allowedMethods  << "\n";
	// 		std::cerr << j << "	location_autoindex			" << config->servers[i].locations[j].autoindex  << "\n";
	// 		std::cerr << j << "	location_index			" << config->servers[i].locations[j].index  << "\n";
	// 		std::cerr << j << "	location_path			" << config->servers[i].locations[j].path << "\n";
	// 		std::cerr << j << "	location_redirectFrom			" << config->servers[i].locations[j].redirectFrom << "\n";
	// 		std::cerr << j << "	location_redirectTo			" << config->servers[i].locations[j].redirectTo << "\n";
	// 	}
	// }
	
	configFile.close();    
	return (config);
}
