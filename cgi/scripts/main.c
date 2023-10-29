#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    char buffer[1024];
    bzero(buffer, 1024);
    int r = read(0, buffer, 1024);
    write(1, "Content-Type: text/html\r\n\r\n", 27);
    write(1, "<html><body>", 12);
    write(1, buffer, r);
    write(1, "</body></html>", 14);
    return 0;
}