#include "parsingHeader.hpp"

// void	func() {
// 	system("leaks webservParsing");
// }


void	printConfigStruct(t_config& config) {
	for (size_t i = 0; i < config.servers.size(); i++) {
		for (std::map<int, std::string>::iterator it = config.servers[i].errorPages.begin(); it != config.servers[i].errorPages.end(); it++)
			std::cout << "errorPage: ---" << it->first << ", " << it->second << "---\n";
		std::cout << "index: ---" << config.servers[i].index  << "---\n";
		std::cout << "port: ---" << config.servers[i].port  << "---\n";
		std::cout << "root: ---" << config.servers[i].root  << "---\n";
		std::cout << "servername: ---" << config.servers[i].serverName << "---\n";
		std::cout << "clientMaxBodySize: ---" << config.servers[i].clientMaxBodySize << "---\n\n";
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
void	printConfigStruct(t_config& config);

int getLenOfMatching(std::string requestPath, std::string locationPath) {
	if (locationPath.size() > requestPath.size())
		return -1;
	int i = 0;
	while (i < (int)requestPath.size() & requestPath[i] == locationPath[i])
		i++;
	if (i == (int)locationPath.size() && (requestPath[i] == '/' || i == (int)requestPath.size()))
		return (i);
	return (-1);
}

int	main(int argc, char* argv[]) {
	// atexit(func);
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;

	if (argc <= 2) {
		t_config config = parseConFile(argv[1]);
		printConfigStruct(config);
		// t_request request;
		// std::ifstream reqOutfile("reqOut");
		// if (!reqOutfile.is_open()) {
		// 	std::cerr << "Error opening HTML file" << std::endl;
		// 	return 1;
		// }

		// std::ostringstream buffer;
		// buffer << reqOutfile.rdbuf();
		// reqOutfile.close();
		// std::string	buf = buffer.str();
		// requestParse(request, buf);
	} else {
		usage(argv[0]);
	}
	return (0);
}
