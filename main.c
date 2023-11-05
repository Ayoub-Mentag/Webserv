#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int fd1 = open("test1", O_RDONLY);
    int fd2 = open("test2", O_RDONLY);
    int fd3 = open("test3", O_RDONLY);
    int fd4 = open("test4", O_RDONLY);
    close(fd1);
    int fd5 = open("test5", O_RDONLY);
    int fd6 = open("test6", O_RDONLY);

    int fd7 = open("test7", O_RDONLY);
    int fd8 = open("test8", O_RDONLY);
    int fd9 = open("test9", O_RDONLY);
    int fd10 = open("test10", O_RDONLY);
    int fd11 = open("test11", O_RDONLY);
    int fd12 = open("test12", O_RDONLY);

    printf("%d\n", fd1);
    printf("%d\n", fd2);
    printf("%d\n", fd3);
    printf("%d\n", fd4);
    printf("%d\n", fd5);
    printf("%d\n", fd6);

    printf("%d\n", fd7);
    printf("%d\n", fd8);
    printf("%d\n", fd9);
    printf("%d\n", fd10);
    printf("%d\n", fd11);
    printf("%d\n", fd12);
}