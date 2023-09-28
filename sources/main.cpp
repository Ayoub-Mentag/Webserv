#include "parsingHeader.hpp"

void	func() {
	system("leaks webservParsing");
}

int	main(int argc, char* argv[]) {
	// atexit(func);
	// argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;

	// if (argc <= 2) {
	// 	t_config config = parseConFile(argv[1]);
	// 	t_request request;
	// 	std::ifstream reqOutfile("reqOut");
	// 	if (!reqOutfile.is_open()) {
	// 		std::cerr << "Error opening HTML file" << std::endl;
	// 		return 1;
	// 	}

	// 	std::ostringstream buffer;
	// 	buffer << reqOutfile.rdbuf();
	// 	reqOutfile.close();
	// 	std::string	buf = buffer.str();
	// 	requestParse(request, buf);
	// } else {
	// 	usage(argv[0]);
	// }
	// return (0);
}