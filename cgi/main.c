#include <unistd.h>
#include <stdlib.h>

int main(int ac, char **av) {
    write(1, "Hello from cgi", 14);
    return 0;
}