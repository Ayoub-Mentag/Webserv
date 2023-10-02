#include <parsingHeader.hpp>

#define DEFAULT_PORT				8080
#define DEFAULT_ROOT				"."

void	initWIthDefault(t_config& config) {
	for (size_t i = 0; i < config.servers.size(); i++) {
		if (config.servers[i].root.empty()) {
			config.servers[i].root = DEFAULT_ROOT;
		}
		if (config.servers[i].port < -1) {
			config.servers[i].port = DEFAULT_PORT;
		}
		if (config.servers[i].allowedMethods.size() == 0) {
			config.servers[i].allowedMethods.push_back("GET");
			config.servers[i].allowedMethods.push_back("POST");
			config.servers[i].allowedMethods.push_back("DELETE");
		}
		for (size_t j = 0; j < config.servers[i].locations.size(); j++) {
			if (config.servers[i].locations[j].root.empty()) {
				config.servers[i].locations[j].root = DEFAULT_ROOT;
			}
		}
	}
}


t_config	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	config;
	std::string	line;
	std::string res;

	std::ifstream configFile(file);
	if (!configFile.is_open()) {
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
		res += line + UNKNOWN_CHAR;
	}
	if (!bracketsBalance(res)) {
		std::cerr << RED "Error: " GREEN "Unclosed bracket in configuration file." RESET_COLOR<< std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	splitServerBlocks(config, res);
	configFile.close();
	initWIthDefault(config);
	return (config);
}