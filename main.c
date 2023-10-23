#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Example environment variables to set
    char *envp[] = {
        "ACCEPT=text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8",
        "ACCEPT_ENCODING=gzip,deflate,br",
        "ACCEPT_LANGUAGE=en-US,en",
        // Add other environment variables here
        NULL
    };

    char *argv[] = {"./cgi/python-cgi" ,"cgi/scripts/testenv.py", NULL};

    if (execve(argv[0], argv, envp) == -1) {
        perror("execve");
        return 1;
    }

    return 0;
}
