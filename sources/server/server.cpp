#include <serverHeader.hpp>

void Server::bindServerWithAddress()
{
	int result = bind(this->serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void Server::setPortOfListening()
{
	if (listen(serverSocketfd, 5) == -1)
	{
		throw std::runtime_error("listen");
	}
}

Server::Server(t_config& config) : config(config) 
{
	if ((this->serverSocketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket() : ");
		throw std::runtime_error("socket()");
	}
	int opt = 1;
	if (setsockopt(this->serverSocketfd, SOL_SOCKET,  SO_REUSEPORT , &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);
	FD_ZERO(&current_sockets);
	FD_SET(serverSocketfd, &current_sockets);
	bindServerWithAddress();
	setPortOfListening();
}

Server::~Server()
{
	close(serverSocketfd);
}




Request	*Server::getRequest() {
	/**
	 * @test we will work on some examples without getting the request from browser
	*/
	std::string							buffer;

	// Boundary
	// buffer = "POST /endpoint HTTP/1.1\r\nHost: localhost:8080\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nkey1=value1&key2=value2\r\n";
	// buffer = (
	// 	"POST /upload-endpoint HTTP/1.1\r\nHost: localhost:8080\r\nContent-Type: multipart/form-data boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n\r\n"
	// 	"----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
	// 	"Content-Disposition: form-data; name=\"file\"; filename=\"example.txt\"\r\n\r\n"
	// 	"Content-Type: text/plain\r\n"
	// 	"File content goes here\r\n"
	// 	"----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
	// );

	// Chunked
	buffer = (
		"POST /chunked-endpoint HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Type: application/octet-stream\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"20\r\n"
		"This is the first chunk of data.\r\n"
		"30\r\n"
		"This is the second chunk.\r\n"
	);


	// print buffer after the checking
	checkRequest(buffer);
	Request *request = requestParse(buffer);
	return (request);
}

fd_set Server::getReadyFds() {
	fd_set ready_socket;
	FD_ZERO(&ready_socket);
	ready_socket = current_sockets;
	//read, write, error , timout
	if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL) < 0)
	{
		perror("Select : ");
	}
	return ready_socket;
}

void Server::acceptNewConnection()
{
	int clientFd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;
	if ((clientFd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
		perror("Accept : ");
	}
	else {
		std::cout << "A new connection Accepted " << std::endl;
		FD_SET(clientFd, &current_sockets);
	}
}

void	printMap(std::map<std::string, std::string>	m) {
	for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++) {
		std::cout << "Key " << it->first << " Value " << it->second << std::endl;
	}
}

void	printRequest(Request* request) {
	REQUEST_TYPE type = request->getTypeOfRequest();
	if (type == NONE) {
		std::cout << "Unknown type of request " << std::endl;
		return ;
	}
	std::cout << "*************Head\n";
	printMap(request->getHead());

	std::cout << "\n*************Body\n";
	if (type == POST_CHUNKED) {
		ChunkedRequest *c = dynamic_cast<ChunkedRequest *>(request);
		std::cout << c->getEntityPost() << std::endl;
	} else if (type == POST_BOUNDARY)
	{
		BoundaryRequest *b = dynamic_cast<BoundaryRequest *>(request);
		for (size_t i = 0; i < b->getBody().size(); i++) {
			printMap(b->getBody()[i]);
		}
	}
	else if (type == POST_SIMPLE) {
		SimpleRequest *s = dynamic_cast<SimpleRequest *>(request);
		std::cout << s->getEntityPost() << std::endl;
	}
}

void Server::serve()
{
	Request *request = getRequest();
	printRequest(request);
}
