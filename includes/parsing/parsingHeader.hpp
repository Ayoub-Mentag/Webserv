#pragma once 

# include <sys/socket.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <signal.h>
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <sstream>

/* ****** COLORS ****** */
# define RESET_COLOR "\033[0m"
# define RED          "\033[1;31m"
# define GREEN        "\033[1;32m"
# define YELLOW       "\033[1;33m"
# define ORANGE       "\033[1;34m"
# define MAGENTA      "\033[1;35m"
# define CYAN         "\033[1;36m"
# define WHITE        "\033[1;37m"

# define ALLOWED_URI_CHARS      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%"
# define UNKNOWN_CHAR           (char)200
# define DEFAULT_CONFIG_FILE    "./configFiles/def.conf"

# define NO_LOC_PATH    		RED "Error: " GREEN << "location has no path." << RESET_COLOR << "\n"
# define INVALID_ARGUMENT		RED "Error: " GREEN << key << " Invalid argument." << RESET_COLOR << "\n"
# define NO_VALUE				RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n"
# define PRINT_LINE_AND_FILE	YELLOW "[file: " << __FILE__ << "]\n[line: " << __LINE__ << "]\n" RESET_COLOR
# define INVALID_METHOD			RED "Error: " GREEN "Invalid method." << RESET_COLOR << "\n"
# define INVALID_LOC_DIRECTIVE	RED "Error: " GREEN "Invalid location Directive." << RESET_COLOR << "\n"
# define EXPECTED_SEM			RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n"
# define INVALID_DIRECTIVE		RED "Error: " GREEN "Invalid Directive." << RESET_COLOR << "\n"


# define HEADER_REQUEST 1
# define HEADER_BODY    2

class Request;

typedef struct s_request {
	std::vector<std::string>    head;
	std::string                 body;
}   t_request;



void	                                checkRequest(std::string &buffer);
Request                                *requestParse(std::string buffer);


class Request {
	protected :
		std::map<std::string, std::string>	head;
		std::string							entityPost;
	public :
		Request();
		virtual void                        parseBody(std::string body) = 0;
		void                                setHead( std::map<std::string, std::string>  head);
		std::string							getEntityPost() const;
		void								setEntityPost(std::string body);

};

class BoundaryRequest : public Request {
	private :
		// the body will conteain the header elements 
		// and the entityPost
		std::vector<std::map<std::string, std::string> > body;
		std::string                         boundary;

	public :
		BoundaryRequest();
		void                                                parseBody(std::string body);
		void                                                setBoundary(std::string boundary);
		void                                                setBody(std::vector<std::map<std::string, std::string> >);
		void	                                            printRequest();
		// std::string                                       getBoundary();

};

class SimpleRequest : public Request {
	public :
		SimpleRequest();
		void                                                parseBody(std::string body);
};

class ChunkedRequest : public Request {
	public :
		ChunkedRequest();
		void                                                parseBody(std::string body);
};

typedef struct LocationDirectives {
	bool						autoindex;
	std::string					path;
	std::string					root;
	std::string					index;
	std::string					redirectFrom;
	std::string 				redirectTo;
	std::vector<std::string>	allowedMethods;
	std::map<int, std::string> 	errorPages;
	int					    	clientMaxBodySize;
	std::string                 cgiExecutable;
	bool                        isCgi;
}								t_location;

typedef struct ServerDirectives {
	int								port;
	int					    		clientMaxBodySize;
	std::map<int, std::string> 		errorPages;
	std::string						serverName;
	std::string						root;
	std::string						index;
	std::vector<std::string>        allowedMethods;
	std::vector<t_location>     	locations;
}									t_server;

typedef struct ConfigSettings {
	std::vector<t_server>	servers;
}							t_config;

/* extra functions */
std::string					        trim(const std::string& str);
bool						        bracketsBalance(const std::string& str);

/* Parsing Directives */
void                                getErrorPages(std::string& value, std::string& key, std::map<int, std::string>& errorPages);
std::vector<std::string>	        getAllowedMethods(std::string& value, std::string& key);
std::string					        getIndex(std::string& value, std::string& key);
std::string					        getRoot(std::string& value, std::string& key);
std::string					        getCgiExecutable(std::string& value, std::string& key);
std::string					        getServerName(std::string& value, std::string& key);
void						        getRedirect(std::string& value, std::string& key, std::string& redirectFrom, std::string& redirectTo);
bool						        getAutoIndex(std::string& value, std::string& key);
int							        getPort(std::string& value, std::string& key);
int							        getLimitClientBody(std::string& value, std::string& key);

/* Parse Locations */
void						        splitLocationBlocks(t_server& server, std::string res);

/* Parse Servers */
void						        splitServerBlocks(t_config& config, std::string res);

/* Parse Config File */
t_config					        parseConFile(const char* file);

/* Parse Request */
std::map<std::string, std::string>  fillContentTypeMap();
void	correctPath(std::string& path); // just for now

