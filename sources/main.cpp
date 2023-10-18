#include <Parsing.hpp>
#include <Server.hpp>
#include <Utils.hpp>

static void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
	exit(1);
}

int	main(int argc, char* argv[]) {
	t_config	config;

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
