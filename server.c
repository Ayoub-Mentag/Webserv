// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 8080

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

    bind(serverSocketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("bounded\n");
    listen(serverSocketfd, 1);
    printf("Listning ... \n");
    clientSocketfd = accept(serverSocketfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    char received[1024];
    read(clientSocketfd, received, sizeof(received));
    if (!strcmp(received, "/"))
        sendFile("index.html", clientSocketfd);
    else if (!strcmp(received, "/test/test.html"))
        sendFile(received, clientSocketfd);
    else
        write(clientSocketfd, "404 NOT FOUND :(\n");
}