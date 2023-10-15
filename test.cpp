#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <stack>
#include <map>

class Base {
	public :
		virtual ~Base() {}
};

class A : public Base{
	private :
		int a;
	public :
		~A() {}
		A() : a(10){}
		int getA() {
			return a;
		}
};
class B : public Base{
	private :
		int a;
	public :
		~B() {}
		B() : a(20){}
		int getA() {
			return a;
		}
};


int main() {
	Base *a = new A();
	A  *casted = dynamic_cast<A*>(a);
	std::cout << casted->getA() <<std::endl;

	return 0;
}