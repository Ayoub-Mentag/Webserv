#include "serverPars.hpp"

# define INVALID_ARGUMENT		RED "Error: " GREEN << key << " Invalid argument." << RESET_COLOR << "\n"
# define NO_VALUE				RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n"
# define PRINT_LINE_AND_FILE	YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR
# define INVALID_METHOD			RED "Error: " GREEN "Invalid method." << RESET_COLOR << "\n"
# define INVALID_LOC_DIRECTIVE	RED "Error: " GREEN "Invalid location Directive." << RESET_COLOR << "\n"
# define EXPECTED_SEM			RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n"
# define INVALID_DIRECTIVE		RED "Error: " GREEN "Invalid Directive." << RESET_COLOR << "\n"

void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
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
                return (false); // Closing bracket with no matching opening bracket
            }
            if ((c == '}' &&  stack.top() == '{') || (c == ']' &&  stack.top() == '[')) {
                stack.pop();
            } else {
                return (false); // Mismatched brackets
            }
        }
    }
    return (stack.empty()); // Check for extra opening brackets
}



/* ************************** Parse Directives ****************************** */
std::vector<std::string>		getAllowedMethods(std::string& value, std::string& key) {
	std::string					token;
	std::vector<std::string>	allowedMethods;
		
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::istringstream			toSplit(value);
	while (std::getline(toSplit, token, ' ')) {
		if (token == "") continue;
		if (token != "GET" && token != "POST" && token != "DELETE") {
			std::cerr << INVALID_METHOD;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		allowedMethods.push_back(token);
	}
	return (allowedMethods);
}

std::string	getIndex(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

bool	getAutoIndex(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	if (value == "0" || value == "false" || value == "FALSE") {
		return (false);
	} else if (value == "1" || value == "true" || value == "TRUE") {
		return (true);
	} else {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

void	getRedirect(std::string& value, std::string& key, std::string& redirectFrom, std::string& redirectTo) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	size_t first = value.find_first_of(",");
	if (first != std::string::npos) {
		redirectFrom = trim(value.substr(0, first));
		redirectTo = trim(value.substr(first + 1, -1));
	}
	if (redirectFrom == "" || redirectTo == "") {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

std::string	getRoot(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

std::string getServerName(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

int	getPort(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	if (value.find_first_not_of("0123456789") != value.npos) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (atoi(value.c_str()));
}
std::map<int, std::string>	getErrorPages(std::string& value, std::string& key) {
	std::map<int, std::string> errorPages;
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::string			token;
	std::istringstream	em(value);
	while (std::getline(em, token, '&')) {
		token = trim(token);
		size_t first = token.find_first_of(",");
		if (first == std::string::npos) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		std::string	errorIndex = trim(token.substr(0, first));
		std::string errorValue = trim(token.substr(first + 1, -1));
		if (errorIndex == "" || errorValue == ""
			|| errorIndex.find_first_not_of("0123456789") != value.npos) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		errorPages[std::atoi(errorIndex.c_str())] = errorValue;
	}
	
	return (errorPages);
}




/* ************************** Parse Location ****************************** */
static void	parseLocationDirectives(std::string& key, std::string value, t_location& location) {
	if (value[value.length() - 1] != ';') {
		std::cerr << EXPECTED_SEM;
		std::cerr << PRINT_LINE_AND_FILE;
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
		location.root = getRoot(value, key);
	} else {
		std::cerr << INVALID_LOC_DIRECTIVE;
		std::cerr << PRINT_LINE_AND_FILE;
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
		location.path = trim(res.substr(0, findBrack));
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
		std::cerr << EXPECTED_SEM;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	size_t last = value.find_last_not_of(';');
	if (last != value.npos && value[last + 1] == ';')
		value = value.substr(0, last + 1);

	std::cerr << "server_value:" << value << "\n";
	if (key == "server_name") {
		server.serverName = getServerName(value, key);
	} else if (key == "listen") {
		server.port = getPort(value, key);
	} else if (key == "root") {
		server.root = getRoot(value, key);
	} else if (key == "index") {
		server.index = getIndex(value, key);
	} else if (key == "error_page") {
		server.errorPages = getErrorPages(value, key);
	} else {
		std::cerr << INVALID_DIRECTIVE << PRINT_LINE_AND_FILE;
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
void	printConfigStruct(t_config& config);

t_config	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	config;
	std::string	line;
	std::string res;

	std::ifstream configFile(file);
	if (!configFile.is_open()) {
		std::cerr << RED "Error:" GREEN "Failed to open the configuration file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
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
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	splitServerBlocks(config, res);
	printConfigStruct(config);
	configFile.close();    
	return (config);
}










































void	printConfigStruct(t_config& config) {
	for (size_t i = 0; i < config.servers.size(); i++) {
		for (std::map<int, std::string>::iterator it = config.servers[i].errorPages.begin(); it != config.servers[i].errorPages.end(); it++)
			std::cout << "errorPage: ---" << it->first << ", " << it->second << "---\n";
		std::cout << "index: ---" << config.servers[i].index  << "---\n";
		std::cout << "port: ---" << config.servers[i].port  << "---\n";
		std::cout << "root: ---" << config.servers[i].root  << "---\n";
		std::cout << "servername: ---" << config.servers[i].serverName << "---\n\n";
		for (size_t j = 0; j < config.servers[i].locations.size(); j++) {
			for (size_t k = 0; k < config.servers[i].locations[j].allowedMethods.size(); k++) {
				std::cout <<"	location_allowedMethods ---" << config.servers[i].locations[j].allowedMethods[k]  << "---\n";
			}
			std::cout << "\n";
			std::cout << "	location_autoindex ---" << config.servers[i].locations[j].autoindex  << "---\n";
			std::cout << "	location_index ---" << config.servers[i].locations[j].index  << "---\n";
			std::cout << "	location_path ---" << config.servers[i].locations[j].path << "---\n";
			std::cout << "	location_redirectFrom ---" << config.servers[i].locations[j].redirectFrom << "---\n";
			std::cout << "	location_redirectTo ---" << config.servers[i].locations[j].redirectTo << "---\n";
		}
		std::cout << "\n";
	}
}