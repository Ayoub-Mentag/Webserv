#include <unistd.h>
#include <stdlib.h>

int main(int ac, char **av) {
    int fd = atoi(av[1]);
    write(fd, "Hello from cgi", 14);
    return 0;
}