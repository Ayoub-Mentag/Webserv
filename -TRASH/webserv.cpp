#include <iostream>
#include <sys/socket.h>  // for socket(), bind(), listen(), accept()
#include <netinet/in.h>  // for sockaddr_in
#include <unistd.h>      // for close()
#include <fstream>
#include <sstream>
#include <string>
#include <arpa/inet.h>

int	main() {
	int	serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		std::cout << "cannot create socket" << std::endl;
		return 0;
	}
	sockaddr_in	socketAddress;
	socketAddress.sin_port = htons(8080);
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (const struct sockaddr*)&socketAddress, sizeof(struct sockaddr_in)) < 0) {
		std::cout << "cannot bind socket" << std::endl;
		return 0;
	}
	if (listen(serverSocket, 3) < 0) 
	{ 
		perror("In listen");
		exit(EXIT_FAILURE); 
	}
	//*********************************************
	while (true) {
        // Accept incoming connections
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            perror("Accepting connection failed");
            continue;
        }

        // Respond to the client
		// const char* response = "HTTP/1.1 404 Not Found\r\nContent-Length: 24\r\n\r\n404 - Page Not Found"; //"HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
		// Read the HTML file
		std::ifstream html_file("index.html");
		if (!html_file.is_open()) {
			std::cerr << "Error opening HTML file" << std::endl;
			return 1;
		}

		std::ostringstream html_content_stream;
		html_content_stream << html_file.rdbuf();
		html_file.close();

		std::string html_content = html_content_stream.str();

		// Construct the HTTP response
		std::ostringstream response_stream;
		response_stream << "HTTP/1.1 200 OK\r\n";
		response_stream << "Content-Type: text/html\r\n";
		response_stream << "Content-Length: " << html_content.length() << "\r\n";
		response_stream << "\r\n";
		response_stream << html_content;

		std::string response = response_stream.str();

		int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);

		// Close the client socket
		close(clientSocket);
	}
	close(serverSocket);
}

	// sockaddr_in	clientAddress;
	// socklen_t	clientSocketLen = sizeof(clientAddress);
	// int			clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSocketLen);
	// if (clientSocket == -1) {
	// 	perror("Accepting connection failed");
	// 	return -1;  // Handle error
	// }

	// char buffer[1024];
	// int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	// if (bytesRead == -1) {
	// 	perror("Receiving data failed");
	// 	return -1;  // Handle error
	// }

	// // Process received data
	// const char* response = "HTTP/1.1 200 OK\r\n"
    //                   "Content-Type: text/html\r\n"
    //                   "\r\n"
    //                   "<html><body><h1>Hello, World!</h1></body></html>";

	// int bytesSent = send(clientSocket, response, strlen(response), 0);
	// if (bytesSent == -1) {
	// 	perror("Sending data failed");
	// 	return -1;  // Handle error
	// }