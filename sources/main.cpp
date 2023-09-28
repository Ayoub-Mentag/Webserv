#include "serverPars.hpp"

void	func() {
	system("leaks webservParsing");
}

int	main(int argc, char* argv[]) {
	// atexit(func);
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;

	if (argc <= 2) {
		t_config* config = parseConFile(argv[1]);
		delete config;
	} else {
		usage(argv[0]);
	}
	return (0);
}