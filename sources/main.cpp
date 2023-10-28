#include <Parsing.hpp>
#include <Server.hpp>
#include <Utils.hpp>

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
int	main(int argc, char* argv[]) {
	t_config	config;
	std::cout << "Hello";

	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	if (argc <= 2) {
		config = parseConFile(argv[1]);
		try {
			Server server(config);
			while (1) {
				server.serve();
			}
		} catch (std::exception &ex) {
			std::cout << "--" << ex.what() << std::endl;
		}
	} else {
		usage(argv[0]);
	}
	return (0);
}
