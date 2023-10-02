#include <headerFile.hpp>

static void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
	exit(1);
}

// static void	printConfigStruct(t_config& config) {
// 	for (size_t i = 0; i < config.servers.size(); i++) {
// 		for (std::map<int, std::string>::iterator it = config.servers[i].errorPages.begin(); it != config.servers[i].errorPages.end(); it++)
// 			std::cout << "errorPage: ---" << it->first << ", " << it->second << "---\n";
// 		std::cout << "index: ---" << config.servers[i].index  << "---\n";
// 		std::cout << "port: ---" << config.servers[i].port  << "---\n";
// 		std::cout << "root: ---" << config.servers[i].root  << "---\n";
// 		std::cout << "servername: ---" << config.servers[i].serverName << "---\n";
// 		std::cout << "clientMaxBodySize: ---" << config.servers[i].clientMaxBodySize << "---\n\n";
// 		for (size_t j = 0; j < config.servers[i].locations.size(); j++) {
// 			for (size_t k = 0; k < config.servers[i].locations[j].allowedMethods.size(); k++) {
// 				std::cout <<"	location_allowedMethods ---" << config.servers[i].locations[j].allowedMethods[k]  << "---\n";
// 			}
// 			std::cout << "\n";
// 			std::cout << "	location_autoindex ---" << config.servers[i].locations[j].autoindex  << "---\n";
// 			std::cout << "	location_index ---" << config.servers[i].locations[j].index  << "---\n";
// 			std::cout << "	location_path ---" << config.servers[i].locations[j].path << "---\n";
// 			std::cout << "	location_redirectFrom ---" << config.servers[i].locations[j].redirectFrom << "---\n";
// 			std::cout << "	location_redirectTo ---" << config.servers[i].locations[j].redirectTo << "---\n";
// 		}
// 		std::cout << "\n";
// 	}
// }

int	main(int argc, char* argv[]) {
	// char		buffer[MAX_LEN];
	t_config	config;

	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	if (argc <= 2) {
		config = parseConFile(argv[1]);
		try {
			Server server(config);
			while (1)
				server.serve();
		} catch (std::exception &ex) {
			std::cout << ex.what() << std::endl;
		}
	} else {
		usage(argv[0]);
	}
	return (0);
}
