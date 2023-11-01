#include <string>
#include <iostream>

class Base {
	private :
		int a;
	public : 
		Base(int a) : a(a) {}
		int getA() {return a;}
};

class Child : public Base{

	public : 
		Child(int a) : Base(a) {}
};



class Child2 : public Child{
	public : 
		Child2(int a) : Child(a) {}
};

int main() {
	Child c(10);
	std::cout << c.getA();
    return 0;
}