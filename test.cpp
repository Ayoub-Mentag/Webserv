#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdio>
#include <cerrno>

int main() {
    int fd[2];
    pid_t pid;
    char buffer[4096];

    if (pipe(fd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        return 1;
    }
    if (pid == 0) {
        dup2(fd[0], STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);  // Close the read end of the pipe in the child process
        close(fd[1]);  // Close the write end of the pipe
        char* args[] = {"/System/Library/Frameworks/Python.framework/Versions/2.7/Resources/Python.app/Contents/MacOS/Python", "test.py", NULL};
        execve(args[0], args, NULL);
        perror("Execve");
    } else {
        // std::cout << fd[1];
        write(fd[1], "photo=test.jpg&email_address=user@example.com", 41);
        wait(0);
    }

    bzero(buffer, sizeof(buffer));
    ssize_t bytesRead = read(fd[0], buffer, sizeof(buffer));
        close(fd[0]);  // Close the read end of the pipe
        close(fd[1]);  // Close the write end of the pipe in the parent process
    if (bytesRead != -1) {
        std::cout << buffer << "--------" << std::endl;
    } else {
        perror("Read failed");
    }
}
