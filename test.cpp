#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#define NOT_EXIST -1
int main() {
    // std::string inputString = "Hello,World,How,Are,You";
    // std::istringstream iss(inputString);
    // std::vector<std::string> tokens;

    // while (std::getline(iss, inputString, ',')) {
    //     tokens.push_back(inputString);
    // }

    // // Now 'tokens' vector contains individual strings after splitting.

    // // Display the result
    // for (size_t i = 0; i < tokens.size(); ++i) {
    //     std::cout << tokens[i] << std::endl;
    // }
    char buffer[1024];

    read(NOT_EXIST, buffer , 1024);
    write(1, buffer, 1024);
    // std::cout << "--- " << buffer << std::endl;
    return 0;
}