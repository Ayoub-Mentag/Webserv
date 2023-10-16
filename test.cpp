#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <stack>
#include <map>

class Base {
	public :
		int a;
		int b;
		int c;
	public :
		Base(int a) : a(a){}
		~Base() {}

};

std::ostream& operator<<(std::ostream& os, const Base& b)
{
    os << b.a;
    return os;
}


#include <stdio.h>

int main() {
	return 0;
}