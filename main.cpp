
#define TEST_AUTO_GENERATED 0

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
	var y = 1;
	print(y + 1);

	var x = 1;
	x = "hey var";
	x = Test();
	print(x); // Object : class Test
	Test* t = x.as<Test>();
	x = Vect2f();
	var is_vec = isinstance(x, Vect2f);
	print(is_vec);

	Array arr;
	arr.push_back(1);
	arr.push_back("test");
	arr.push_back(2.0);
	var varr = arr;
	print(varr);

	return 0;
}