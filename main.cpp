
#define TEST_AUTO_GENERATED 1

#if (TEST_AUTO_GENERATED)
#define VAR_IMPLEMENTATION
#include "var.h"
#else
#include "_var.h"
#endif

#define print(x) std::cout << (x) << std::endl;

class Test
{
public:
	int x, y;
};


int main()
{
	var a = "asdf";
	a == Test();

	var x = 1;
	x = "hey var";
	x = Test();
	print(x); // Object : class Test
	Test* t = x.as<Test>();
	var is_vec = x.is<Vect2f>();
	print(is_vec); // false
	return 0;
}