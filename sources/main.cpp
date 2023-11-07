#include <Parsing.hpp>
#include <Server.hpp>
#include <Utils.hpp>

int nfds = -1;

static void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
	exit(1);
}

// TODO: Reads the request body, if any (length specified by the Content-Length header)
// TODO: A few methods (e.g., GET) forbid entity body data in request messages.
// Before the web server can deliver content to the client, it needs to identify the source
// of the content, by mapping the URI from the request message to the proper content
// or content generator on the web server.

typedef struct s_server_client {
	Server	*server;
	int		clientIndex;
} t_server_client;


t_server_client 	getCurrentServerAndClient(std::vector<Server*>& servers, int fd) {
	t_server_client current;
	current.server = NULL;
	current.clientIndex = -1;

	for (size_t serverIndex = 0; serverIndex < servers.size(); serverIndex++) {
		if (fd == servers[serverIndex]->getServerSocketFd())
		{
			current.server = servers[serverIndex];
			current.clientIndex = -1;
			return (current);
		}
		for (size_t clientIndex = 0; clientIndex < servers[serverIndex]->getClients().size(); clientIndex++) {
			if (fd == servers[serverIndex]->getClients()[clientIndex].getFd()) {
				current.server = servers[serverIndex];
				current.clientIndex = clientIndex;
				return (current);
			}
		}
	}
	return (current);
}

int getMax(fd_set *mySet) {
	int max = -1;
	for (int fd = 0 ; fd < FD_SETSIZE; fd++) {
		if (FD_ISSET(fd, mySet))
		{
			std::cout << "---fd " << fd << std::endl;
			max = (fd > max) ? fd : max;
		}
	}
	// std::cout << "max " << max << std::endl;
	return (max);
}

int	main(int argc, char* argv[]) {
	t_config				config;
	std::vector<Server *>	servers;
	fd_set					mySet;
	fd_set					readyToReadFrom;
	int						newFd;
	int						nfds = -1;

	// add a struct of set to keep track of the ready W/R and the whole fds
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	if (argc <= 2) {
		try {
			config = parseConFile(argv[1]);
			FD_ZERO(&mySet);
			// init servers
			for (size_t i = 0; i < config.servers.size(); i++) {
				Server *server = new Server(config.servers[i]);
				newFd = server->getServerSocketFd();
				if (newFd > nfds)
					nfds = newFd;
				FD_SET(newFd, &mySet);
				servers.push_back(server);
			}
			while (1) {
				FD_ZERO(&readyToReadFrom);
				readyToReadFrom = mySet;
				if (select(nfds + 1, &readyToReadFrom, NULL, NULL, NULL) <= 0) {
					perror("Select ");
					std::cout << errno << std::endl;
					break;
				}
				// std::cout << nfds;
				for (int fd = 0; fd <= nfds; fd++) {
					if (FD_ISSET(fd, &readyToReadFrom)) {
						t_server_client current = getCurrentServerAndClient(servers, fd);
						if (current.server) {
							if (current.clientIndex == -1) {
								newFd = current.server->acceptNewConnection();
								if (newFd > nfds)
									nfds = newFd;
								// std::cout << "newFd " << newFd <<std::endl;
								FD_SET(newFd, &mySet);
							}
							else {
								current.server->dealWithClient(current.clientIndex);
							}
						}
					}
				}
			}
		} catch (std::exception &ex) {
			std::cout << "--" << ex.what() << std::endl;
		}
	} else {
		usage(argv[0]);
	}
	return (0);
}