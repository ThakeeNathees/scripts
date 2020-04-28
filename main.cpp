
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


	Vect2f v1(2, 3);
	Vect2f v2(2, 2);
	std::cout << v1 * v2 << std::endl;

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