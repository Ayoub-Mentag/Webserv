#include <Parsing.hpp>


void	initWIthDefault(t_config& config) {
	for (size_t i = 0; i < config.servers.size(); i++) {
		if (config.servers[i].root.empty()) {
			// config.servers[i].root = DEFAULT_ROOT;
		}
		if (config.servers[i].allowedMethods.size() == 0) {
			config.servers[i].allowedMethods.push_back("GET");
			// config.servers[i].allowedMethods.push_back("POST");
			// config.servers[i].allowedMethods.push_back("DELETE");
		}
	}
}

t_config	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	config;
	std::string	line;
	std::string	res;

	std::ifstream configFile(file);
	if (configFile.fail()) {
		std::cerr << RED "Error: " GREEN "Failed to open the configuration file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	while (std::getline(configFile, line)) {
		size_t commentPos = line.find("#");
		if (commentPos != std::string::npos) {
			line.erase(commentPos, line.length());
		}
		line = trim(line);
		if (line.empty()) {
			continue ;
		}
		res += line + "\n";
		// res += line + UNKNOWN_CHAR;
	}
	if (res.empty() || res.find('{') == res.npos) {
		std::cerr << RED "Error: " GREEN "invalid config file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	if (!bracketsBalance(res)) {
		std::cerr << RED "Error: " GREEN "Unclosed bracket in configuration file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	while (res.size() > 0) {
		res = trim(res);
		line = res.substr(0, 6);
		if (trim(line) != "Server") {
			std::cerr << GREEN << "No Server Block Found!\n" << RESET_COLOR;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}

		res = res.substr(6, -1);
		res = trim(res);
		if (res[0] != '{') {
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		size_t countOpenBrackets = std::count(res.begin(), find(res.begin(), res.end(), '}'), '{');
		size_t blockLen = 0;
		line = res;
		while (countOpenBrackets) {
			size_t j = line.find('}');
			if (j != line.npos) {
				line.erase(0, j + 1);
				blockLen += j + 1;
				countOpenBrackets--;
			}
		}
		line = res.substr(0, blockLen);
		std::cout << line + "\n";
		std::string::iterator currentPos = res.begin();
		std::string::iterator simPos = std::find(currentPos, res.end(), ';');
		if (std::find(currentPos, simPos, '{') != res.end()) {
			// get location blocks
		} else {
			currentPos = simPos;
			// parse server line
		}

		// parse the block then erase it
		res.erase(res.find(line), line.length());
	}

	return (config);
}


	// size_t findServ = res.find("Server");
	// if (findServ == res.npos) {
	// 	std::cerr << GREEN << "No Server Block Found!\n" << RESET_COLOR;
	// 	std::cerr << PRINT_LINE_AND_FILE;
	// 	exit(1);
	// }
	// splitServerBlocks(config, res);
	// configFile.close();
	// initWIthDefault(config);