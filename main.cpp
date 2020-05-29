
//#define TEST_AUTO_GENERATED

#if (TEST_AUTO_GENERATED)
#define VAR_IMPLEMENTATION
#include "var.h"
#else
#include "_var.h"
#endif
using namespace varh;
#define print(x) std::cout << (x) << std::endl

class A{
	virtual void f(){}
};
class B:public A {};
class C:public A {};
class CC :public C {};

class X : public Object {
	virtual String to_string() const { return "X"; }
	virtual operator String() const { return to_string(); }

	virtual bool get(const String& p_name, var& r_val) const { return false; }
	virtual bool set(const String& p_name, const var& p_val) { return false; }
	virtual bool has(const String& p_name) const { return false; }
};


int main()
{

	return 0;
}