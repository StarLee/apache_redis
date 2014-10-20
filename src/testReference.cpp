#include <string>
#include <iostream>
void test_point(std::string * str)
{
	str=new std::string("ppppp");
}
void test_reference(std::string * &str)
{
	str=new std::string("rrrrr");
}
int main()
{
	std::string *a=new std::string("aaaaaa");
	test_point(a);
	std::cout<<*a<<std::endl;
	std::string *b=new std::string("bbbbb");
	test_reference(b);
	std::cout<<*b<<std::endl;
	return 0;
}
