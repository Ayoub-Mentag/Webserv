#include "parsingHeader.hpp"

#include <vector>
#include <algorithm>

#define MAX_LEN 1024
void	func() {
	system("leaks webservParsing");
}
void	printConfigStruct(t_config& config);

void	checkRequest(t_request &request, t_config config)
{
	int i = 0;
	// exist on the server names in the config file
	// then nginx will route the request to the default server for this port
	for (; i < (int)config.servers.size(); i++) {
		if (config.servers[i].serverName == request.serverName)
			break ;
	}
	if (i >= (int)config.servers.size())
		throw std::runtime_error("Server name not found");
	

	//matching the routes


}

int	main(int argc, char* argv[]) {
	// atexit(func);
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	char buffer[MAX_LEN];
	if (argc <= 2) {
		t_config config = parseConFile(argv[1]);
		printConfigStruct(config);
		t_request request;
		requestParse(request, buffer);

	} else {
		usage(argv[0]);
	}
	return (0);
}