#include "../includes/serverPars.hpp"

void	usage(const char* programName) {
	std::cerr << GREEN_TEXT "Usage: " RED_TEXT << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	exit(1);
}

// void	parseConFile(const char* file) {
// 	std::ifstream	conFile;
// 	std::string		line;

// 	conFile.open(file);
// 	if (!conFile) {
// 		std::cerr << RED_TEXT "Failed to open the file." RESET_COLOR << std::endl;
// 		exit(1);
// 	}
// 	std::getline(conFile, line);
// 	while (std::getline(conFile, line)) {
// 		if (line.find("server {") != std::string::npos) {
// 			// Start of a server block, create a new t_server instance
// 			t_server server;
// 			// Parse and populate server-specific settings
// 			// Add the server to the config.servers vector
// 		} else if (line.find("location {") != std::string::npos) {
// 			// Start of a location block, create a new t_location instance
// 			t_location location;
// 			// Parse and populate location-specific settings
// 			// Add the location to the current server's locations vector
// 		} else if (line.find("}") != std::string::npos) {
// 			// End of a block (server or location), continue processing
// 		} else {
// 			// Handle other directive-specific parsing and population logic
// 		}

// 	}
	
// 	conFile.close();
// }
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

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

std::string trim(const std::string& str) {
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
static void	splitBlocks(std::string res, const std::string& block);

static void	parseBlock(std::string res, const std::string& block) {
	// std::cout << "res: " << res << "\n";
	// std::string line = res.substr(0, res.find(";"));
	// std::cout << "line: " << line << "\n";
	// while (res.size()) {
		
	// }

	if (block == "Server") {
		size_t findBrack = res.find("{");
		if (findBrack != res.npos)
			res = res.substr(findBrack + 1, -1);
		std::vector<std::string>	tokens;
		std::istringstream			tokenStream(res);
		std::string					token;
		while (std::getline(tokenStream, token, ';')) {
			if (token.find("location") != token.npos)
				break ;
		    tokens.push_back(token);
			std::cout << "server_token: " << token << "\n";
		}
		splitBlocks(res.substr(res.find("location"), -1), "location");

	} else {
		size_t findBrack = res.find("{");
		if (findBrack != res.npos) {
			std::string path = res.substr(0, findBrack);
			res = res.substr(findBrack + 1, -1);
			std::cerr << "path: " << path << "\n";
		}
		std::vector<std::string>	tokens;
		std::istringstream			tokenStream(res);
		std::string					token;
		while (std::getline(tokenStream, token, ';')) {
			if (token == "}" || token == "}}") {
				continue ;
			}
		    tokens.push_back(token);
			std::cout << "token_location: " << token << "\n";
		}
	}
	
}

static void	splitBlocks(std::string res, const std::string& block) {
		while (res.size()) {
		std::string	serverBlock;
		size_t fserv = res.find(block);
		if (fserv != std::string::npos) {
			res = res.substr(block.length(), std::string::npos);
			fserv = res.find(block);
			if (fserv != std::string::npos) {
				parseBlock(res.substr(0, fserv), block);
				std::cout << block << ": " << res.substr(0, fserv) << "\n";
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			parseBlock(res, block);
			std::cout << block << ": " << res << "\n";
			break ;
		}
	}
}

void	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	config;
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
	splitBlocks(res, "Server");
	//     // Split the line into tokens (directive and arguments)
	//     std::vector<std::string> tokens = split(line);
	//     // Parse directives and their arguments
	//     if (!tokens.empty()) {
	//         std::string directive = tokens[0];
			
	//         // Handle server block
	//         if (directive == "server") {
	//             // If we were parsing a previous server block, push it to the config
	//             if (currentServer.port != 0) {
	//                 config.servers.push_back(currentServer);
	//             }
	//             // Initialize a new server block
	//             currentServer = t_server();
	//         }
	//         // Handle location block
	//         else if (directive == "location") {
	//             // If we were parsing a previous location block, push it to the current server's locations
	//             if (!currentLocation.path.empty()) {
	//                 currentServer.locations.push_back(currentLocation);
	//             }
	//             // Initialize a new location block
	//             currentLocation = t_location();
	//             currentLocation.path = tokens[1];
	//         }
	//         // Parse other directives
	//         else {
	//             // Parse and store other directives and their arguments in the appropriate data structure
	//             // parseAndStoreDirective(tokens, currentServer, currentLocation);
	//         }
	//     }
	// }
	
	// // Push the last server and location blocks into the config
	// if (currentServer.port != 0) {
	//     config.servers.push_back(currentServer);
	// }
	// if (!currentLocation.path.empty()) {
	//     currentServer.locations.push_back(currentLocation);
	// }

	configFile.close();    
}
