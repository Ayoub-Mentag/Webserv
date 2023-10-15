#include <parsingHeader.hpp>


std::vector<std::string>	splitLine(std::string line, std::string delimiter) {
	std::vector<std::string>	result;
	std::string					tmpLine;
	size_t						end;

	end = 0;
	while (line.size()) {
		end = line.find(delimiter);
		if (end == std::string::npos)
			end = line.length();
		tmpLine = line.substr(0, end);
		result.push_back(tmpLine);
        if (end + delimiter.size() > line.size())
            break ;
        line = &line[end + delimiter.size()];
	}
	return (result);
}


std::string	trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return ("");
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}
