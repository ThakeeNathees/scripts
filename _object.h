//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#ifndef OBJECT_H
#define OBJECT_H

#include "varhcore.h"

#define REGISTER_CLASS(m_class, m_inherits)                                                          \
public:                                                                                              \
	typedef m_inherits Super;                                                                        \
	static constexpr const char* get_class_name_s() { return STR(m_class); }                         \
	virtual const char* get_class_name() const override { return get_class_name_s(); }               \
	static constexpr const char* get_parent_class_name_s() { return STR(m_inherits); }               \
	virtual const char* get_parent_class_name() const override { return get_parent_class_name_s(); } \
	static ptr<Object> __constructor() { return newptr<m_class>(); }                                 \
	static void _bind_data(NativeClasses* p_native_classes)

namespace varh {

class NativeClasses;

class Object {
public:

	// Operators.
	operator String()  const { return to_string(); }
	Object& operator=(const Object& p_copy) = default;
	//var operator()(stdvec<var>& p_vars);

	bool operator==(const var& p_other) const { return __eq(p_other); }
	bool operator!=(const var& p_other) const { return !operator == (p_other); }
	bool operator<=(const var& p_other) const { return __lt(p_other) || __eq(p_other); }
	bool operator>=(const var& p_other) const { return __gt(p_other) || __eq(p_other); }
	bool operator< (const var& p_other) const { return __lt(p_other); }
	bool operator> (const var& p_other) const { return __gt(p_other); }

	var operator+(const var& p_other) const;
	var operator-(const var& p_other) const;
	var operator*(const var& p_other) const;
	var operator/(const var& p_other) const;

	var& operator+=(const var& p_other);
	var& operator-=(const var& p_other);
	var& operator*=(const var& p_other);
	var& operator/=(const var& p_other);

	var operator[](const var& p_key) const;
	var& operator[](const var& p_key);

	// Virtual methods.
	// These double underscore methdos will be used as operators callback in the compiler.

	static var call_method(ptr<Object> p_self, const String& p_name, stdvec<var*>& p_args);  // instance.p_name(args)
	static var get_member(ptr<Object> p_self, const String& p_name);
	static void set_member(ptr<Object> p_self, const String& p_name, var& p_value);

	virtual var __call(stdvec<var*>& p_vars);                                                // instance(args)
	virtual var __call_method(const String& p_method_name, stdvec<var*>& p_args);            // class's own implementation of call_method();
	virtual var __get_member(const String& p_member_name);
	virtual void __set_member(const String& p_member_name, var& p_value);

	virtual var __iter_begin();
	virtual bool __iter_has_next();
	virtual var __iter_next();

	virtual var __get_mapped(const var& p_key) const;
	virtual void __set_mapped(const var& p_key, const var& p_val);
	virtual int64_t __hash() const;

	virtual var __add(const var& p_other) const;
	virtual var __sub(const var& p_other) const;
	virtual var __mul(const var& p_other) const;
	virtual var __div(const var& p_other) const;

	virtual var& __add_eq(const var& p_other);
	virtual var& __sub_eq(const var& p_other);
	virtual var& __mul_eq(const var& p_other);
	virtual var& __div_eq(const var& p_other);

	virtual bool __gt(const var& p_other) const;
	virtual bool __lt(const var& p_other) const;
	virtual bool __eq(const var& p_other) const;

	virtual String to_string() const { return String::format("[Object:%i]", this);  }

	// Methods.
	virtual ptr<Object> copy(bool p_deep)         const { throw VarError(VarError::NOT_IMPLEMENTED, "Virtual method \"copy()\" not implemented on type \"Object\"."); }
	constexpr static  const char* get_class_name_s()              { return "Object"; }
	virtual const char* get_class_name()          const { return get_class_name_s(); }
	static ptr<Object> __constructor() { return newptr<Object>(); }
	constexpr static  const char* get_parent_class_name_s()       { return ""; }
	virtual const char* get_parent_class_name()   const { return get_parent_class_name_s(); }

	static const stdvec<const MemberInfo*> get_member_info_list(const Object* p_instance);
	static const MemberInfo* get_member_info(const Object* p_instance, const String& p_member);

	static void _bind_data(NativeClasses* p_native_classes);

private:
	friend class var;
};

}


#endif //OBJECT_H