#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <parsingHeader.hpp>
std::vector<std::string>	splitLine(std::string line, std::string delimiter);
std::string					trim(const std::string& str);
std::string                 to_string(int num);
void                        printRequest(Request* request);