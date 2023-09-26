#include <../includes/server.hpp>
/*
http {
	GET /search?q=query&limit=10 HTTP/1.1
	Host: example.com
}
*/

// add this macros to server.hpp later please
#define PORT 8080
#define BACKLOG 5
#define MAX_LEN 1024

void sendFile(int fd, std::string fileName)
{
    std::ifstream	inFile;
	std::string		send;

	int n;
	inFile.open(fileName);
	if (inFile.fail())
		throw std::runtime_error(fileName + " does not exist");
	// int filefd = open(file, O_RDONLY);
	// char send[1024];
	// bzero(send, sizeof(send));

	// while ((n = read(filefd, send, sizeof(send))) > 0)
	while (std::getline(inFile, send))
	{
		write(fd, send.c_str(), send.length());
		bzero((void *)send.c_str(), send.length());
		// printf("server %d %s\n", n, send);
	}
	inFile.close();
	// close(filefd);
}


std::string readFromFd(int fd)
{
	char buffer[MAX_LEN];
	std::string str;
	int n;

	read(fd, buffer, sizeof(buffer));
	str.append(buffer);
	// while ((n = read(fd, buffer, sizeof(buffer))) > 0)
	// {
	// 	std::cout << " ------ " << n << std::endl;
	// 	str.append(buffer);
	// 	bzero(buffer, MAX_LEN);
	// }
	return str;
}

int main()
{
	// creation of socket
	int serverSocketfd, clientSocketfd;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	serverSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);


	int bind_result;
    int wait_seconds = 5; // Adjust this value as needed

	bind_result = bind(serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (bind_result == -1) {
		return (perror("Bind"), 1);
	}
	std::cout << "Bounded successfully .." << std::endl;
	if (listen(serverSocketfd, 5) == -1)
		return (perror("Listen : "), 1);
	std::cout << "Listned successfully .." << std::endl;
	if ((clientSocketfd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1)
		perror("Accept : ");
	std::cout << "A client has been entered" << std::endl;

	char buffer[MAX_LEN];
	bzero(buffer, MAX_LEN);
	std::string request = readFromFd(clientSocketfd);
	std::cout << request;
	close(clientSocketfd);
	// parseRequest(request);






	// if (!strncmp(received, "GET / HTTP/1.1", 14))
	// {
	// 	write(clientSocketfd, "HTTP/1.1 200 OK\r\n\r\n", 19);
	// 	sendFile(clientSocketfd, "");
	// }
	// else if (!strncmp(received, "GET /test/test.html HTTP/1.1", 28))
	// {
	// 	printf("hello\n");
	// 	write(clientSocketfd, "HTTP/1.1 200 OK\r\n\r\n", 19);
	// 	sendFile(clientSocketfd, "test/test.html");
	// }
	// else
	// 	write(clientSocketfd, "404 NOT FOUND :(\n", 17);
	// close(clientSocketfd);
	// close(serverSocketfd);
	// exit(0);
}