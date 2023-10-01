#include "parsingHeader.hpp"

#include <vector>
#include <algorithm>

#define MAX_LEN 1024
void	func() {
	system("leaks webservParsing");
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
	} else {
		usage(argv[0]);
	}
	return (0);
}