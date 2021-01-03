
#define TEST_AUTO_GENERATED

#ifdef TEST_AUTO_GENERATED
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

class MyClass : public Object {
public:
	virtual String to_string() override {
		return "MyClass";
	}
	~MyClass() {
		print("~MyClass");
	}

};


int main() {
	var v;
	v = 3.14;           print(v); // prints 3.14 float
	v = "hello world!"; print(v); // prints the String

	v = String("string"); v = Map();
	v = Array(1, 2.3, "hello world!", Array(4, 5, 6));

	class Aclass : public Object {
	public: String to_string() const { return "Aclass"; }
	};
	v = newptr<Aclass>(); print(v); // prints Aclass
}