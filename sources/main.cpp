#include <Parsing.hpp>
#include <Server.hpp>
#include <Utils.hpp>

static void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
	exit(1);
}

void	initServers(t_config& config, std::vector<Server>& servers, fd_set& allSocketFds) {
	for (size_t i = 0; i < config.servers.size(); i++) {
		Server server(config);
		FD_SET(server.getServerSocketFd(), &allSocketFds);
		servers.push_back(server);
	}
}


void	dealWithClient(int i) {
	Client client = clients[i];


}

void	handleConnection(std::vector<Server> servers, int fd, fd_set& allSocketFds) {
	for (size_t i = 0; i < servers.size(); i++) {
		if (servers[i].getServerSocketFd() == fd) {
			FD_SET(servers[i].acceptNewConnection(), &allSocketFds);
		}
		else {
			for (size_t j = 0; servers[i].getClients().size(); j++) {
				if (servers[i].getClients()[j].getClientFd() == fd) {
					servers[i].dealWithClient(j);
				}
			}
		}

	}
}




// TODO: Reads the request body, if any (length specified by the Content-Length header)
// TODO: A few methods (e.g., GET) forbid entity body data in request messages.
// Before the web server can deliver content to the client, it needs to identify the source
// of the content, by mapping the URI from the request message to the proper content
// or content generator on the web server.
int	main(int argc, char* argv[]) {
	t_config			config;
	std::vector<Server>	servers;
	// add a struct of set to keep track of the ready W/R and the whole fds
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	if (argc <= 2) {
		try {
			fd_set allSocketFds;
			fd_set readyToReadFrom;
			fd_set readyToWrite;
			config = parseConFile(argv[1]);
			initServers(config, servers, allSocketFds);
			while (1) {
				FD_ZERO(&readyToReadFrom);
				FD_ZERO(&readyToWrite);
				readyToReadFrom = allSocketFds;
				if (select(FD_SETSIZE, &readyToReadFrom, &readyToWrite, NULL, NULL) < 0) {
					perror("Select : ");
				}
				for (int fd = 0; fd < FD_SETSIZE; fd++) {
					if (FD_ISSET(fd, &readyToReadFrom)) {
						handleConnection(fd, servers, allSocketFds);
						FD_CLR(fd, &readyToReadFrom);
					}
					if (FD_ISSET(fd, &readyToWrite)) {
						
					}
				}
				// server.serve(readyToReadFrom, readyToWrite);
			}
		} catch (std::exception &ex) {
			std::cout << "--" << ex.what() << std::endl;
		}
	} else {
		usage(argv[0]);
	}
	return (0);
}
