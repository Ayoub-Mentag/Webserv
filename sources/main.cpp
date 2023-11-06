#include <Parsing.hpp>
#include <Server.hpp>
#include <Utils.hpp>

static void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
	exit(1);
}

void    printAllowedMethods(const std::vector<std::string>& vec, int f) {
	for (size_t i = 0; i < vec.size(); i++) {
		std::cout << (f ? "		" : "") << "allowedMethods: ---" << vec[i] << "---\n";
	}
}

void    printErrorPages(const std::map<int, std::string>& errP, int f) {
	for (std::map<int, std::string>::const_iterator i = errP.begin(); i != errP.end(); i++) {
		std::cout << (f ? "		" : "") << "errPageCode --" << i->first << "--- ErrorPage: ---" << i->second << "--\n";
	}
}

void    printLocations(const std::vector<t_location>& locations) {
	for (size_t i = 0; i < locations.size(); i++) {
		std::cout << "	location index: *" << i << "*\n";
		std::cout << "		path: --" << locations[i].path << "--\n";
		std::cout << "		clientMaxBodySize: --" << locations[i].clientMaxBodySize << "--\n";
		std::cout << "		root: --" << locations[i].root << "--\n";
		std::cout << "		index: --" << locations[i].index << "--\n";
		std::cout << "		redirCode: --" << locations[i].redirectionCode << "--\n";
		std::cout << "		redirTo: --" << locations[i].redirectTo << "--\n";
		std::cout << "		isCgi: --" << locations[i].isCgi << "--\n";
		std::cout << "		cgiExecutable: --" << locations[i].cgiExecutable << "--\n";
		std::cout << "		autoindex: --" << locations[i].autoindex << "--\n";
		printAllowedMethods(locations[i].allowedMethods, 1);
		printErrorPages(locations[i].errorPages, 1);
		std::cout << "\n";
	}
	
}

void    printConfig(const t_config& config) {
	std::vector<t_server> s = config.servers;
	for (size_t i = 0; i < s.size(); i++) {
		std::cout << "port: --" << s[i].port << "--\n";
		std::cout << "ipAddress: --" << s[i].ipAddress << "--\n";
		std::cout << "clientMaxBodySize: --" << s[i].clientMaxBodySize << "--\n";
		std::cout << "serverName: --" << s[i].serverName << "--\n";
		std::cout << "root: --" << s[i].root << "--\n";
		std::cout << "index: --" << s[i].index << "--\n";
		std::cout << "redirCode: --" << s[i].redirectionCode << "--\n";
		std::cout << "redirTo: --" << s[i].redirectTo << "--\n";
		printAllowedMethods(s[i].allowedMethods, 0);
		printErrorPages(s[i].errorPages, 0);
		std::cout << "\n";
		printLocations(s[i].locations);
		std::cout << "\n ********************************* \n";
	}
	
}

int	main(int argc, char* argv[]) {
	t_config	config;

	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	if (argc <= 2) {
		config = parseConFile(argv[1]);
		// printConfig(config);
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
