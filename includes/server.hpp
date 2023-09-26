#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>



typedef struct RequestStruct {
	std::string method;
	std::string path;
	std::map<std::string, std::string> queryParams;
	std::string host;
	std::string httpVersion;
} t_request;

