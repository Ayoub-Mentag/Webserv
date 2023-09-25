#include "../includes/serverPars.hpp"

void	usage(const char* programName) {
    std::cerr << GREEN_TEXT "Usage: " RED_TEXT << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	exit(1);
}

void	parseConFile(const char* file) {
	std::ifstream	conFile;

	conFile.open(file);
	if (!conFile) {
		std::cerr << RED_TEXT "Failed to open the file." RESET_COLOR << std::endl;
		exit(1);
	}
	
	

	conFile.close();
}
