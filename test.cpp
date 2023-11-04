#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/select.h>

int nfds = 1;

void fun() {
    nfds--;
}

int main() {
    nfds++;
    fun();
    fun();
    std::cout << nfds << std::endl;
    return 0;
}