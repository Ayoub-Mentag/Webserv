#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SERVER_PORT 80

#define MAXLINE 4096

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80);  // Replace with the port your server is listening on
    inet_pton(AF_INET, av[1], &serverAddr.sin_addr);
    if (connect(clientFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        perror("error");
    write(clientFd, "GET / HTTP/1.1\r\n\r\n", 18);

    char recieved[1024];
    int n;

    while (( n = read(clientFd, recieved, 1024)) > 0)
    {
        if (!strcmp(recieved, "\0"))
            break;
        printf("%s %d----\n", recieved, n);
        bzero(recieved, sizeof(recieved));
    }
    close(clientFd);

}