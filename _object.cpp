//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020-2021 Thakee Nathees
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

#include "var.h"
#include "_error.h"
//#include "core/native.h"

namespace varh {


ptr<Object> Object::copy(bool p_deep) /*const*/ { THROW_ERROR(Error::NOT_IMPLEMENTED, "Virtual method \"copy()\" not implemented on type \"Object\"."); }

Object::operator String() { return to_string(); }
var Object::operator()(stdvec<var*>& p_args) { return __call(p_args); }

bool Object::operator==(const var& p_other) /*const*/ { return __eq(p_other); }
bool Object::operator!=(const var& p_other) /*const*/ { return !operator == (p_other); }
bool Object::operator<=(const var& p_other) /*const*/ { return __lt(p_other) || __eq(p_other); }
bool Object::operator>=(const var& p_other) /*const*/ { return __gt(p_other) || __eq(p_other); }
bool Object::operator< (const var& p_other) /*const*/ { return __lt(p_other); }
bool Object::operator> (const var& p_other) /*const*/ { return __gt(p_other); }

var Object::operator+(const var& p_other) /*const*/ { return __add(p_other); }
var Object::operator-(const var& p_other) /*const*/ { return __sub(p_other); }
var Object::operator*(const var& p_other) /*const*/ { return __mul(p_other); }
var Object::operator/(const var& p_other) /*const*/ { return __div(p_other); }

var& Object::operator+=(const var& p_other) { return __add_eq(p_other); }
var& Object::operator-=(const var& p_other) { return __sub_eq(p_other); }
var& Object::operator*=(const var& p_other) { return __mul_eq(p_other); }
var& Object::operator/=(const var& p_other) { return __div_eq(p_other); }

#define _OBJ_THROW_NOT_IMPL(m_name)\
	THROW_ERROR(Error::NOT_IMPLEMENTED, String("operator " #m_name " not implemented on base ") + get_type_name() + ".")

var Object::operator[](const var& p_key) const { _OBJ_THROW_NOT_IMPL(operator[]); }
var& Object::operator[](const var& p_key) { _OBJ_THROW_NOT_IMPL(operator[]); }

var Object::__call(stdvec<var*>& p_vars) { _OBJ_THROW_NOT_IMPL(__call()); }
var Object::call_method(const String& p_method_name, stdvec<var*>& p_args) {
	THROW_ERROR(Error::ATTRIBUTE_ERROR, String::format("type %s has no method named \"%s\".", get_type_name(), p_method_name.c_str()));
}
//var Object::operator()(stdvec<var>& p_vars) { return __call(p_vars); }

var  Object::get_member(const String& p_member_name) {
	THROW_ERROR(Error::ATTRIBUTE_ERROR, String::format("type %s has no member named \"%s\"", get_type_name(), p_member_name.c_str()));
}
void Object::set_member(const String& p_member_name, var& p_value) {
	THROW_ERROR(Error::ATTRIBUTE_ERROR, String::format("type %s has no writable member named \"%s\".", get_type_name(), p_member_name.c_str()));
}

var Object::__get_mapped(const var& p_key) /*const*/ { _OBJ_THROW_NOT_IMPL(__get_mapped()); }
void Object::__set_mapped(const var& p_key, const var& p_val) { _OBJ_THROW_NOT_IMPL(__set_mapped()); }
int64_t Object::__hash() /*const*/ { _OBJ_THROW_NOT_IMPL(__hash()); }

var  Object::__iter_begin() { _OBJ_THROW_NOT_IMPL(__iter_begin()); }
bool Object::__iter_has_next() { _OBJ_THROW_NOT_IMPL(__iter_has_next()); }
var  Object::__iter_next() { _OBJ_THROW_NOT_IMPL(__iter_next()); }

var Object::__add(const var& p_other) /*const*/ { _OBJ_THROW_NOT_IMPL(__add()); }
var Object::__sub(const var& p_other) /*const*/ { _OBJ_THROW_NOT_IMPL(__sub()); }
var Object::__mul(const var& p_other) /*const*/ { _OBJ_THROW_NOT_IMPL(__mul()); }
var Object::__div(const var& p_other) /*const*/ { _OBJ_THROW_NOT_IMPL(__div()); }

var& Object::__add_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__add_eq()); }
var& Object::__sub_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__sub_eq()); }
var& Object::__mul_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__mul_eq()); }
var& Object::__div_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__div_eq()); }

bool Object::__gt(const var& p_other) /*const*/ { _OBJ_THROW_NOT_IMPL(__gt()); }
bool Object::__lt(const var& p_other) /*const*/ { _OBJ_THROW_NOT_IMPL(__lt()); }
bool Object::__eq(const var& p_other) /*const*/ {
	if (p_other.get_type() != var::OBJECT) return false;
	return this == p_other.operator ptr<Object>().get();
}


String Object::to_string() /*const*/ { return String::format("[%s:%i]", get_type_name(), this); }
void* Object::get_data() { return (void*)this; }

// call_method() should call it's parent if method not exists.
var Object::call_method_s(ptr<Object>& p_self, const String& p_name, stdvec<var*>& p_args) {
	ASSERT(false); /*TODO:*/
	return var();
}

var Object::get_member_s(ptr<Object>& p_self, const String& p_name) {
	ASSERT(false); /*TODO:*/
	return var();
}


void Object::set_member_s(ptr<Object>& p_self, const String& p_name, var& p_value) {
	ASSERT(false); /*TODO:*/
}

#undef _OBJ_THROW_NOT_IMPL

} // namespace carbon