
#define TEST_AUTO_GENERATED 0

#if (TEST_AUTO_GENERATED)
#define VAR_IMPLEMENTATION
#include "var.h"
#else
#include "_var.h"
#endif

#define print(x) std::cout << x << std::endl;

int main()
{
	var x = 1;
	x = "hello var";
	print(x);
	return 0;
}