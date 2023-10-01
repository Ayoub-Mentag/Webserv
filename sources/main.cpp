#include "parsingHeader.hpp"

<<<<<<< HEAD
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
=======
#include <vector>
#include <algorithm>

#define MAX_LEN 1024
void	func() {
	system("leaks webservParsing");
>>>>>>> matching
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

<<<<<<< HEAD
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
=======

// void	checkRequest(t_request &request, t_config config)
// {
// 	int i = 0;
// 	int j = 0;
// 	int len = 0;
// 	int tmp;
	// std::vector<std::string> locations;
	// locations.push_back("/");
	// locations.push_back("/test");
	// locations.push_back("/test/abc");
	// locations.push_back("/test/abc/efg");
	// locations.push_back("/test/tttt");
	// exist on the server names in the config file
	// then nginx will route the request to the default server for this port
	
	// for (; i < (int)config.servers.size(); i++) {
	// 	if (config.servers[i].serverName == request.serverName)
	// 		break ;
	// }
	// if (i >= (int)config.servers.size())
	// 	throw std::runtime_error("Server name not found");
	// locations = config.servers[i].locations;
	
	// i = -1;
	//matching the routes
	// for (; j < (int)locations.size(); j++)
	// {
	// 	// tmp = getLenOfMatching(request.path, locations[j].path);
	// 	tmp = getLenOfMatching(request.path, locations[j]);
	// 	// std::cout << tmp << std::endl;
	// 	if (tmp > len)
	// 	{
	// 		len = tmp;
	// 		i = j;
	// 	}
	// }
	// if (i == -1)
	// 	throw std::runtime_error("should I show 404 page or the root page");

	// std::string pathToBeLookFor = request.path;
	// pathToBeLookFor.erase(0, locations[i].size());

	// // std::cout << i << "--" << locations[i] << locations[j].size() << std::endl;
	// std::cout << "root + " << pathToBeLookFor << std::endl;
// }

int	main(int argc, char* argv[]) {
	char buffer[MAX_LEN];
	t_config config;
	// atexit(func);
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	// char buffer[MAX_LEN];
	if (argc <= 2) {
		// t_config config = parseConFile(argv[1]);
		// printConfigStruct(config);
		t_request request;
		requestParse(request, buffer);
		request.path = argv[1];
		// checkRequest(request, config);
>>>>>>> matching
	} else {
		usage(argv[0]);
	}
	return (0);
<<<<<<< HEAD
}
=======
}
>>>>>>> matching
