#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create server socket");
        exit(1);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to bind");
        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Failed to listen");
        close(serverSocket);
        exit(1);
    }

    fd_set masterSet, readSet;
    FD_ZERO(&masterSet);
    FD_SET(serverSocket, &masterSet);
    int maxSocket = serverSocket;

    std::vector<int> clientSockets;

    while (true) {
        readSet = masterSet;  // Copy the master set to not modify it

        if (select(maxSocket + 1, &readSet, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i <= maxSocket; i++) {
            if (FD_ISSET(i, &readSet)) {
                if (i == serverSocket) {
                    // New client connection
                    int clientSocket = accept(serverSocket, NULL, NULL);
                    if (clientSocket == -1) {
                        perror("accept");
                    } else {
                        FD_SET(clientSocket, &masterSet);
                        if (clientSocket > maxSocket) {
                            maxSocket = clientSocket;
                        }
                        clientSockets.push_back(clientSocket);
                    }
                } else {
                    // Handle data from a client
                    char buffer[1024];
                    int bytesRead = recv(i, buffer, sizeof(buffer), 0);
                    if (bytesRead <= 0) {
                        // Client closed the connection
                        close(i);
                        FD_CLR(i, &masterSet);
                        clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), i), clientSockets.end());
                    } else {
                        // Process and respond to the request
                        // Example: send a "Hello, World!" response
                        send(i, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!", 51, 0);
                    }
                }
            }
        }
    }

    close(serverSocket);
    return 0;
}
