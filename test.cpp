#include "includes/parsing/parsingHeader.hpp"

int main() {
    std::istringstream iss;
    std::string line = "GET / HTTPVERSION\n";
    std::string l;
    iss.str(line);
    // std::cout << lines[0];
    while (std::getline(iss, l, ' ')) {
        std::cout << l << std::endl;
    }
    return 0;
}