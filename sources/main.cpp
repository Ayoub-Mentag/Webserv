#include "parsingHeader.hpp"

// void	func() {
// 	system("leaks webservParsing");
// }

// int	main(int argc, char* argv[]) {
	// atexit(func);
	// argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;

	// if (argc <= 2) {
	// 	t_config config = parseConFile(argv[1]);
	// 	t_request request;
	// 	std::ifstream reqOutfile("reqOut");
	// 	if (!reqOutfile.is_open()) {
	// 		std::cerr << "Error opening HTML file" << std::endl;
	// 		return 1;
	// 	}

	// 	std::ostringstream buffer;
	// 	buffer << reqOutfile.rdbuf();
	// 	reqOutfile.close();
	// 	std::string	buf = buffer.str();
	// 	requestParse(request, buf);
	// } else {
	// 	usage(argv[0]);
	// }
	// return (0);
// }


#include <sys/socket.h> // for socket()
#include <netinet/in.h> // for sockaddr_in
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/event.h>

# define PORT 8080
// sockets
int	main() {
	int	serverFd = socket(PF_INET, SOCK_STREAM, 0);
	if (serverFd < 0) {
		perror("serverFd");
		exit(1);
	}
	sockaddr_in serverAddr;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;

	int reuse = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
		perror("setsockopt");
		close(serverFd);
		return 1;
	}
	if (bind(serverFd, (struct sockaddr* )&serverAddr, sizeof(serverAddr)) < 0) {
		perror("bind");
		close(serverFd);
		exit(2);
	}	

	if (listen(serverFd, 3) < 0) {
		perror("listen");
		close(serverFd);
		exit(2);
	}
	std::cerr << "Server is listening on port [" << PORT << "]..." << std::endl;
	
	while (true) {
		sockaddr_in	clientAddr;
		socklen_t	clientAddrLength = sizeof(clientAddr);
		int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLength);
		if (clientFd < 0) {
			perror("ClientFd");
			close(serverFd);
			exit(3);
		} else {

			std::cerr << "New connection accepted!" << std::endl;
		}
		char buffer[1024];
		bzero(buffer, 1024);
		int readed = read(clientFd, buffer, 1024);
		if (readed == -1) {
			perror("read");
			close(clientFd);
			continue;
		}
		buffer[readed] = 0;
		printf("%s", buffer);fflush(stdout);
		close(clientFd);
	}
	close(serverFd);
}
