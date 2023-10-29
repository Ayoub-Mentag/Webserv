#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <Parsing.hpp>
std::vector<std::string>	splitLine(std::string line, std::string delimiter);
std::string					trim(const std::string& str);
std::string                 to_string(int num);
// void                        printRequest(Request* request);

bool	bracketsBalance(const std::string& str);
void	printMap(std::map<std::string, std::string>	m);
void printVectorOfString(std::vector<std::string> v);