#include <parsingHeader.hpp>

static void	parseServerDirectives(std::string& key, std::string& value, t_server& server) {
	if (value[value.length() - 1] != ';') {
		std::cerr << EXPECTED_SEM;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	size_t last = value.find_last_not_of(';');
	if (last != value.npos && value[last + 1] == ';')
		value = value.substr(0, last + 1);

	if (key == "server_name") {
		server.serverName = getServerName(value, key);
	} else if (key == "listen") {
		server.port = getPort(value, key);
	} else if (key == "root") {
		server.root = getRoot(value, key);
	} else if (key == "index") {
		server.index = getIndex(value, key);
	} else if (key == "error_page") {
		server.errorPages = getErrorPages(value, key);
	} else if (key == "limit_client_body") {
		server.clientMaxBodySize = getLimitClientBody(value, key);
	} else if (key == "allowed_methods") {
		server.allowedMethods = getAllowedMethods(value, key);
	} else {
		std::cerr << INVALID_DIRECTIVE << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

static void	fillServerStruct(t_server& server, std::vector<std::string>& tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i].find("location") != tokens[i].npos)
				break ;
		std::istringstream tokenStream(tokens[i]);
		std::string key;
		std::string value;
		std::getline(tokenStream, key, '=');
		std::getline(tokenStream, value);
		key = trim(key);
		value = trim(value);
		parseServerDirectives(key, value, server);  
	}
}

static t_server	parseServerBlock(std::string res) {
	std::vector<std::string>	serverTokens;
	t_server					server;

	server.clientMaxBodySize = -1;
	server.port = -1;
	size_t findBrack = res.find("{");
	if (findBrack != res.npos) {
		res = res.substr(findBrack + 1, -1);
	}
	if (res[0] == UNKNOWN_CHAR)
		res[0] = ' ';
	std::istringstream			tokenStream(res);
	std::string					token;
	while (std::getline(tokenStream, token, UNKNOWN_CHAR)) {
		if (token.find("location") != token.npos) {
			break ;
		}
		if (token[0] == '}' && token[token.length() - 1] == '}') {
			continue ;
		}
		serverTokens.push_back(token);  
	}
	fillServerStruct(server, serverTokens);
	size_t	floc = res.find("location");
	if (floc != res.npos)
		splitLocationBlocks(server, res.substr(floc, -1));
	return (server);
}

void	splitServerBlocks(t_config& config, std::string res) {
	while (res.size()) {
		std::string	serverBlock;
		size_t fserv = res.find("Server");
		if (fserv != std::string::npos) {
			res = res.substr(6, std::string::npos);
			fserv = res.find("Server");
			if (fserv != std::string::npos) {
				config.servers.push_back(parseServerBlock(res.substr(0, fserv)));
				res = res.substr(fserv, std::string::npos);
			}
		} else if (fserv == std::string::npos && res.size()) {
			config.servers.push_back(parseServerBlock(res));
			break ;
		}
	}
}
