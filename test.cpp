#include <iostream>
#include <string>
#include <vector>

int	main() {
	std::vector<std::string>	locationPath;
	std::string					requestPath = "/www/data/";

	locationPath.push_back("/");
	locationPath.push_back("/www");
	locationPath.push_back("/www/data/test");
	locationPath.push_back("/www/data/test/file");
	locationPath.push_back("/www/data");

	std::string lookFor = requestPath;
	while (lookFor.size() > 0) {
		std::vector<std::string>::iterator it = std::find(locationPath.begin(), locationPath.end(), lookFor);
		if (it != locationPath.end()) {
			std::cout << *it << std::endl;
			return (0);
		}

		int last = lookFor.find_last_of('/');
		if (last == lookFor.npos)
			break ;
		lookFor.erase(last, -1);
		// std::cout << &lookFor[last] << std::endl;
	}
	if (requestPath[0] == '/' && lookFor.empty())
		std::cout << lookFor + '/' << std::endl;
	return (0);

	std::cout << "Not Found" << std::endl;
	

	return (0);
}
