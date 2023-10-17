#include <utilsHeader.hpp>

std::string	to_string(int num) {
	std::stringstream	ss;

    ss << num;
	return (ss.str());
}