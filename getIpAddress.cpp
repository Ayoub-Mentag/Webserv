#include <iostream>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>

#include <fstream>
#include <iostream>
#include <string>
int main() {
    const char* hostname = "google.com"; // Replace with the hostname you want to resolve
    struct addrinfo hints, *result, *p;
    int status;

    // Set up hints for the getaddrinfo() function
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // Use TCP

    // Call getaddrinfo to get the address information
    if ((status = getaddrinfo(hostname, NULL, &hints, &result)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    // Iterate through the list of IP addresses and print them
    for (p = result; p != NULL; p = p->ai_next) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
        char ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN);
        std::cout << "IP Address: " << ip_address << std::endl;
    }

    // Free the memory allocated by getaddrinfo
    freeaddrinfo(result);

    // std::string line;
    // std::ifstream in;

    // in.open("server.cpp");
    // while (std::getline(in, line))
    // {
    //     std::cout << line <<std::endl;
    // }
    return 0;
}
