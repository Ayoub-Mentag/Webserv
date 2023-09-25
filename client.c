#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080

#define MAXLINE 4096

int main()
{
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);  // Replace with the port your server is listening on
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(clientFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    write(clientFd, "Hello\n", 6);

}