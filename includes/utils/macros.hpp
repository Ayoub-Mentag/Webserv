#pragma once 
# define REQ_METHOD     	"Request-Method"
# define REQ_PATH           "REQUEST_URI"
# define REQ_HTTP_VERSION   "Server-Protocol"
# define REQ_SERVER_NAME	"Server-Name"
# define REQ_PORT       	"Server-Port"
# define REQ_QUERY_STRING   "QUERY_STRING"
# define REQ_ENTITY_POST    "Entity_Post"
# define REQ_BOUNDARY       "boundary"
# define REQ_TRANSFER       "Transfer-Encoding"
# define REQ_CONTENT_LENGTH "Content-Length"
// Server
# define MAX_LEN			300000
# define PORT				8080
# define BACKLOG			5
# define LAST_SEEN_TIMEOUT	60

# define DEFAULT_400_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>400 Bad Request</h1></body></html>"
# define DEFAULT_403_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>403 Forbidden</h1></body></html>"
# define DEFAULT_404_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>404 Not Found</h1></body></html>"
# define DEFAULT_405_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>405 Method Not Allowed</h1></body></html>"
# define DEFAULT_501_ERROR_PAGE 		"<!DOCTYPE><html><body><h1>501 Not Implemented</h1></body></html>"

# define BAD_REQUEST_STATUS			400
# define NOT_FOUND_STATUS			404
# define METHOD_NOT_ALLOWED_STATUS	405
# define FORBIDDEN_STATUS			403
# define NOT_IMPLEMENTED_STATUS		501
# define MOVED_PERMANENTLY_STATUS	301



// PARSINGTYPE
# define BOUNDARY			1
# define CHUNKED			2
# define BOUNDARYCHUNKED	3


// parsing
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
# define PRINT_LINE_AND_FILE	YELLOW "[" << __FILE__ << ":" << __LINE__ << "]\n" RESET_COLOR
# define INVALID_METHOD			RED "Error: " GREEN "Invalid method." << RESET_COLOR << "\n"
# define INVALID_LOC_DIRECTIVE	RED "Error: " GREEN "Invalid location Directive." << RESET_COLOR << "\n"
# define EXPECTED_SEM			RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n"
# define INVALID_DIRECTIVE		RED "Error: " GREEN "Invalid Directive." << RESET_COLOR << "\n"

# define DEFAULT_PORT				8080
# define DEFAULT_ROOT				"."