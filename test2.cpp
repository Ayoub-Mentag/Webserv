#include <string>
#include <iostream>

class Base {
	private :
		int a;
	public :
		Base(int a) : a(a) {}
		virtual ~Base() {std::cout << "Base Distructor" << std::endl;}
		int getA() {return a;}
};

class Child : public Base{
	public : 
		Child(int a) : Base(a) {}
		virtual ~Child() {std::cout << "Child Distructor" << std::endl;}
};



class Child2 : public Child{
	public : 
		Child2(int a) : Child(a) {}
		~Child2() {std::cout << "Child2 Distructor" << std::endl;}
};

int main() {
	Base *b = new Child(10);
	delete b;
    return 0;
}