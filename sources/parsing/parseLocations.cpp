#include <parsingHeader.hpp>
 
static void	parseLocationDirectives(std::string& key, std::string value, t_location& location) {
	if (value[value.length() - 1] != ';') {
		std::cerr << EXPECTED_SEM;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	size_t last = value.find_last_not_of(';');
	if (last != value.npos && value[last + 1] == ';')
		value = value.substr(0, last + 1);
	if (key == "allowed_methods") {
		location.allowedMethods = getAllowedMethods(value, key);
	} else if (key == "index") {
		location.index = getIndex(value, key);
	} else if (key == "autoindex") {
		location.autoindex = getAutoIndex(value, key);
	} else if (key == "redirect") {
		getRedirect(value, key, location.redirectFrom, location.redirectTo);
	} else if (key == "error_page") {
		getErrorPages(value, key, location.errorPages);
	} else if (key == "limit_client_body") {
		location.clientMaxBodySize = getLimitClientBody(value, key);
	} else if (key == "root") {
		location.root = getRoot(value, key);
	} else {
		std::cerr << INVALID_LOC_DIRECTIVE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}
static void	fillLocationStruct(t_location& location, std::vector<std::string>& tokens) {

	std::string test = "none";
	for (size_t i = 0; i < tokens.size(); i++) {
		std::istringstream	tokenStream(tokens[i]);
		std::string			key;
		std::string			value;

		std::getline(tokenStream, key, '=');
		std::getline(tokenStream, value);
		key = trim(key);
		value = trim(value);
		parseLocationDirectives(key, value, location);
	}
}

static t_location	parseLocationBlock(std::string res) {
	std::vector<std::string>	locationTokens;
	t_location					location;
	std::string					token;

	location.autoindex = 0;
	location.clientMaxBodySize = -1;
	size_t findBrack = res.find("{");
	if (findBrack != res.npos) {
		location.path = trim(res.substr(0, findBrack));
		if (location.path.empty()) {
			std::cerr << NO_LOC_PATH;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		res = res.substr(findBrack + 1, -1); 
	}
	if (res[0] == UNKNOWN_CHAR)
		res[0] = ' ';
	std::istringstream			tokenStream(res);
	while (std::getline(tokenStream, token, UNKNOWN_CHAR)) {
		if (token[0] == '}' && token[token.length() - 1] == '}') {
			continue ;
		}
		locationTokens.push_back(token);
	}
	fillLocationStruct(location, locationTokens);
	return (location);
}

void	splitLocationBlocks(t_server& server, std::string res) {
	while (res.size()) {
		std::string	serverBlock;
		size_t fserv = res.find("location");
		if (fserv != std::string::npos) {
			res = res.substr(9, std::string::npos);
			fserv = res.find("location");
			if (fserv != std::string::npos) {
				server.locations.push_back(parseLocationBlock(res.substr(0, fserv)));
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			server.locations.push_back(parseLocationBlock(res));
			break ;
		}
	}
}
