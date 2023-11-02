#include <iostream>
#include <vector>


class A {
    private :
        int a;
    public :
        A() {std::cout << "Constructor called" << std::endl;}
};

int main() {
    std::vector<A> a;

    return 0;
}