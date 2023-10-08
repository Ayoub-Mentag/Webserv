#include <parsingHeader.hpp>
 
std::vector<std::string>		getAllowedMethods(std::string& value, std::string& key) {
	std::string					token;
	std::vector<std::string>	allowedMethods;
		
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::istringstream			toSplit(value);
	while (std::getline(toSplit, token, ' ')) {
		if (token == "") continue;
		if (token != "GET" && token != "POST" && token != "DELETE") {
			std::cerr << INVALID_METHOD;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		allowedMethods.push_back(token);
	}
	return (allowedMethods);
}

std::string	getIndex(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

bool	getAutoIndex(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	if (value == "0" || value == "false" || value == "FALSE" || value == "off" || value == "OFF") {
		return (false);
	} else if (value == "1" || value == "true" || value == "TRUE" || value == "on" || value == "ON") {
		return (true);
	} else {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

void	getRedirect(std::string& value, std::string& key, std::string& redirectFrom, std::string& redirectTo) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	size_t first = value.find_first_of(",");
	if (first != std::string::npos) {
		redirectFrom = trim(value.substr(0, first));
		redirectTo = trim(value.substr(first + 1, -1));
	}
	if (redirectFrom == "" || redirectTo == "") {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

std::string	getRoot(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

std::string getServerName(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

int	getPort(std::string& value, std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	if (value.find_first_not_of("0123456789") != value.npos) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (atoi(value.c_str()));
}

void	getErrorPages(std::string& value, std::string& key, std::map<int, std::string>& errorPages) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::string			token;
	std::istringstream	em(value);
	while (std::getline(em, token, '&')) {
		token = trim(token);
		size_t first = token.find_first_of(",");
		if (first == std::string::npos) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		std::string	errorIndex = trim(token.substr(0, first));
		std::string errorValue = trim(token.substr(first + 1, -1));
		if (errorIndex == "" || errorValue == ""
			|| errorIndex.find_first_not_of("0123456789") != value.npos) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		// if (errorPages[std::atoi(errorIndex.c_str())].empty()) {
		errorPages[std::atoi(errorIndex.c_str())] = errorValue;
		// }
	}
	// return (errorPages);
}

int	getLimitClientBody(std::string& value, std::string& key) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	value = trim(value);
	char	suffix = value[value.size() - 1];
	int		numericValue = std::atoi(value.c_str());
    switch (suffix) {
        case 'M':
        case 'm':
            return numericValue * 1024 * 1024; // Megabytes to bytes
        case 'K':
        case 'k':
            return numericValue * 1024; // Kilobytes to bytes
        case 'G':
        case 'g':
            return numericValue * 1024 * 1024 * 1024; // Gigabytes to bytes
        default:
            return numericValue; // No suffix, treat as bytes
    }
	return (0);
}
