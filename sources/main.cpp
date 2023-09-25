#include "../includes/serverPars.hpp"


int	main(int argc, char* argv[]) {
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;

	if (argc <= 2) {
		parseConFile(argv[1]);
	} else {
		usage(argv[0]);
	}
	return (0);
}