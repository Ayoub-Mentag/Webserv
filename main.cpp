#include <string>
#include <iostream>


int main()
{
    std::string path = "/test/test1";
    std::string location = "/test";

    path.erase(0, location.size());

    std::cout << path << std::endl;
    return 0;
}