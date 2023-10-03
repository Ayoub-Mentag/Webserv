#include <iostream>
#include <sys/stat.h>
# include <stdlib.h>
# include <unistd.h>
int main(int ac, const char **av) {
	if (access(av[1], F_OK) == 0) {
		struct stat path_stat;
		// Get information about the path
		stat(av[1], &path_stat);
		if (S_ISREG(path_stat.st_mode))
		{
			std::cout << "correctPath : " << std::endl;
			// if (path[path.length() - 1] == '/') {
			// 	path.erase(path.length() - 1);
			// }
		}
	}
}