
//#define TEST_AUTO_GENERATED

#if (TEST_AUTO_GENERATED)
#define VAR_IMPLEMENTATION
#include "var.h"
#else
#include "_var.h"
#endif
using namespace varh;
#define print(x) std::cout << (x) << std::endl

class X :public Object {
public:
	virtual bool get(const String& p_name, var& r_val) const { return false; }
	virtual bool set(const String& p_name, const var& p_val) { return false; }
	virtual bool has(const String& p_name) const { return false; }

	virtual void copy(Object* r_ret, bool p_deep) const {}
};

int main()
{
	try {
		throw VarError(VarError::ZERO_DIVISION, "can't divide by 0.");
	} catch (const VarError & err) {
		print(err.what());
	}
	return 0;
}