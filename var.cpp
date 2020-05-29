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

#include "_var.h"

#define D_VEC(m_vect, m_dim, m_t) STRCAT3(m_vect, m_dim, m_t)

namespace varh {

var var::tmp;

std::ostream& operator<<(std::ostream& p_ostream, const String& p_str) {
	p_ostream << p_str.c_str();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.operator String();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr) {
	p_ostream << p_arr.operator String();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Dictionary& p_dict) {
	p_ostream << p_dict.operator String();
	return p_ostream;
}

// FIXME
static void var_err_callback(const char* p_msg, const char* p_func, const char* p_file, int p_line) {
	std::cout << "ERROR: " << p_msg << std::endl
		<< "\tat: " << p_func << "(" << p_file << ":" << p_line <<")" << std::endl;
	DEBUG_BREAK();
}
static VarErrCallback s_var_err_callback = var_err_callback;
void var_set_err_callback(const VarErrCallback p_callback) {
	s_var_err_callback = p_callback;
}
VarErrCallback var_get_err_callback() {
	return s_var_err_callback;
}

// String -----------------------------------------------

String String::format(const char* p_format, ...) {
	va_list argp;

	va_start(argp, p_format);

	static const unsigned int BUFFER_SIZE = VSNPRINTF_BUFF_SIZE;
	char buffer[BUFFER_SIZE + 1]; // +1 for the terminating character
	int len = vsnprintf(buffer, BUFFER_SIZE, p_format, argp);

	va_end(argp);
	
	if (len == 0) return String();
	return String(buffer);
}

// Array -----------------------------------------------
Array::operator String() const {
	std::stringstream ss;
	ss << "[ ";
	for (unsigned int i = 0; i < _data->size(); i++) {
		ss << _data->operator[](i).operator String();
		if (i != _data->size() - 1) ss << ", ";
		else ss << " ";
	}
	ss << "]";
	return ss.str();
}

bool Array::operator ==(const Array& p_other) const {
	if (size() != p_other.size())
		return false;
	for (size_t i = 0; i < size(); i++) {
		if (operator[](i) != p_other[i])
			return false;
	}
	return true;
}

Array Array::copy(bool p_deep) const {
	Array ret;
	for (size_t i = 0; i < size(); i++) {
		if (p_deep)
			ret.push_back(operator[](i).copy(true));
		else
			ret.push_back(operator[](i));
	}
	return ret;
}

Array Array::operator+(const Array& p_other) const {
	Array ret = copy();
	for (size_t i = 0; i < p_other.size(); i++) {
		ret.push_back(p_other[i].copy());
	}
	return ret;
}

Array& Array::operator+=(const Array& p_other) {
	for (size_t i = 0; i < p_other.size(); i++) {
		push_back(p_other[i].copy());
	}
	return *this;
}

Array& Array::operator=(const Array& p_other) {
	_data = p_other._data;
	return *this;
}

// Dictionary ----------------------------------------
Dictionary::operator String() const {
	std::stringstream ss;
	ss << "{ ";
	for (std::map<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		ss << it->first.operator String() << " : " << it->second.operator String();
		ss << ", ";
	}
	ss << "}";
	return ss.str();
}

Dictionary Dictionary::copy(bool p_deep) const {
	Dictionary ret;
	for (std::map<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (p_deep)
			ret[it->first] = it->second.copy(true);
		else
			ret[it->first] = it->second;
	}
	return ret;
}

var& Dictionary::operator[](const var& p_key) const { return (*_data)[p_key]; }
var& Dictionary::operator[](const var& p_key) { return (*_data)[p_key]; }
std::map<var, var>::iterator Dictionary::begin() const { return (*_data).begin(); }
std::map<var, var>::iterator Dictionary::end() const { return (*_data).end(); }
std::map<var, var>::iterator Dictionary::find(const var& p_key) const { return (*_data).find(p_key); }

bool Dictionary::has(const var& p_key) const { return find(p_key) != end(); }

bool Dictionary::operator ==(const Dictionary& p_other) const {
	if (size() != p_other.size())
		return false;
	for (std::map<var, var>::iterator it_other = p_other.begin(); it_other != p_other.end(); it_other++) {
		std::map<var, var>::iterator it_self = find(it_other->first);
		if (it_self == end()) return false;
		if (it_self->second != it_other->second) return false;

	}
	return true;
}

Dictionary& Dictionary::operator=(const Dictionary& p_other) {
	_data = p_other._data;
	return *this;
}
// Ref -----------------------------------------------

String Ref::to_string() const {
	return (*_data).to_string();
}

Ref::operator String() const {
	return (*_data).operator String();
}

bool operator==(const char* p_cstr, const String& p_str) {
	return p_str == String(p_cstr);
}
bool operator!=(const char* p_cstr, const String& p_str) {
	return p_str != String(p_cstr);
}

// var -----------------------------------------------

void var::clear() {
	clear_data();
	type = _NULL;
}

var var::copy(bool p_deep) const {
	switch (type) {
		case _NULL:
		case BOOL:
		case INT:
		case FLOAT:
		case STRING:
		case VECT2F:
		case VECT2I:
		case VECT3F:
		case VECT3I:
			return *this;
		case ARRAY: return _data._arr.copy(p_deep);
		case DICTIONARY: return _data._dict.copy(p_deep);
		case OBJECT:

			break;
	}
			// TODO:
	return *this;
}

/* constructors */
var::var() {
	_data._bool = false;
	type = _NULL;
}

var::var(const var& p_copy) {
	copy_data(p_copy);
	type = p_copy.type;
}

var::var(bool p_bool) {
	type = BOOL;
	_data._bool = p_bool;
}

var::var(int p_int) {
	type = INT;
	_data._int = p_int;
}

var::var(float p_float) {
	type = FLOAT;
	_data._float = p_float;
}

var::var(double p_double) {
	type = FLOAT;
	_data._float = p_double;
}

var::var(const char* p_cstring) {
	type = STRING;
	new(&_data._string) String(p_cstring);
}

var::var(const String& p_string) {
	type = STRING;
	new(&_data._string) String(p_string);
}

#define VAR_VECT_CONSTRUCTOR(m_dim, m_t, m_T)             \
var::var(const D_VEC(Vect, m_dim, m_t)& p_vect) {         \
	type = D_VEC(VECT, m_dim, m_T);                       \
	std::memcpy(_data._mem, &p_vect, sizeof(_data._mem)); \
}
VAR_VECT_CONSTRUCTOR(2, f, F)
VAR_VECT_CONSTRUCTOR(2, i, I)
VAR_VECT_CONSTRUCTOR(3, f, F)
VAR_VECT_CONSTRUCTOR(3, i, I)
#undef VAR_VECT_CONSTRUCTOR


var::var(const Array& p_array) {
	type = ARRAY;
	_data._arr = p_array;
}

var::var(const Dictionary& p_dict) {
	type = DICTIONARY;
	_data._dict = p_dict;
}

var::var(const Ref& p_obj) {
	type = OBJECT;
	_data._obj = p_obj;
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)             \
var var::operator m_op () {                    \
	switch (type) {                            \
		case INT:  return m_op _data._int;     \
		case FLOAT: return m_op _data._float;  \
		default: VAR_ERR("invalid casting");   \
	}                                          \
	return var();                              \
}

#define VAR_OP_POST_INCR_DECR(m_op)            \
var var::operator m_op(int) {                  \
	switch (type) {                            \
		case INT: return _data._int m_op;      \
		case FLOAT: return _data._float m_op;  \
		default: VAR_ERR("invalid casting");   \
	}                                          \
	return var();                              \
}
VAR_OP_PRE_INCR_DECR(++)
VAR_OP_PRE_INCR_DECR(--)
VAR_OP_POST_INCR_DECR(++)
VAR_OP_POST_INCR_DECR(--)
#undef VAR_OP_PRE_INCR_DECR
#undef VAR_OP_POST_INCR_DECR

var& var::operator=(const var& p_other) {
	copy_data(p_other);
	return *this;
}
var& var::operator=(const Ptr<Object>& p_other) {
	clear_data();
	type = OBJECT;
	_data._obj = p_other;
	return *this;
}

var& var::operator[](const var& p_key) const {
	switch (type) {
		case ARRAY: {
			int index = (int)p_key;
			if (0 <= index && index < (int)_data._arr.size())
				return _data._arr[index];
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				return _data._arr[_data._arr.size() + index];
			VAR_ERR("index error");
		}
		case DICTIONARY:
			return _data._dict[p_key];
	}
	VAR_ERR("invalid operator[]");
	return var::tmp;
}

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL: return false;
		case BOOL: return _data._bool;
		case INT: return _data._int != 0;
		case FLOAT: return _data._float != 0;
		case STRING: return _data._string.size() != 0;

		case VECT2F: return *DATA_PTR_CONST(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR_CONST(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case ARRAY: return !_data._arr.empty();
		case DICTIONARY: return !_data._dict.empty();
		case OBJECT: return _data._obj.is_null();
	}
	VAR_ERR("invalid casting");
	return false;
}

var::operator int() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		case STRING: return  _data._string.to_int();
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator double() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return _data._float;
		case STRING: return  _data._string.to_double();
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator String() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return std::to_string(_data._int);
		case FLOAT: return std::to_string(_data._float);
		case STRING: return _data._string;
		case VECT2F: return (*DATA_PTR_CONST(Vect2f)).operator String();
		case VECT2I: return (*DATA_PTR_CONST(Vect2i)).operator String();
		case VECT3F: return (*DATA_PTR_CONST(Vect3f)).operator String();
		case VECT3I: return (*DATA_PTR_CONST(Vect3i)).operator String();
		case ARRAY: return _data._arr.operator String();
		case DICTIONARY: return _data._dict.operator String();
		case OBJECT: return _data._obj.operator String();
	}
	VAR_ERR("invalid casting");
	return "";
}

#define VAR_VECT_CAST(m_dim, m_t)                                                       \
var::operator D_VEC(Vect, m_dim, m_t)() const {                                         \
	switch (type) {                                                                     \
		case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, f));      \
		case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, i));      \
		default: VAR_ERR("invalid casting");                                            \
	}                                                                                   \
	return D_VEC(Vect, m_dim, m_t)();                                                   \
}
VAR_VECT_CAST(2, f)
VAR_VECT_CAST(2, i)
VAR_VECT_CAST(3, f)
VAR_VECT_CAST(3, i)
#undef VAR_VECT_CAST

var::operator Array() const {
	switch (type) {
		case ARRAY: return _data._arr;
		default: VAR_ERR("invalid casting");
	}
	return Array();
}

var::operator Dictionary() const {
	switch (type) {
		case DICTIONARY: return _data._dict;
		default: VAR_ERR("invalid casting");
	}
	return Dictionary();
}

/* operator overloading */
		/* comparison */
#define VAR_RET_OP(m_op, m_data1, m_type, m_data2, m_cast) \
	case m_type: return (m_cast)_data.m_data1 m_op p_other._data.m_data2
#define VAR_SWITCH_PRIME_TYPES(m_data, m_op)                              \
	switch (p_other.type) {                                               \
		VAR_RET_OP(m_op, m_data, BOOL, _bool, bool);                      \
		VAR_RET_OP(m_op, m_data, INT, _int, int);                         \
		VAR_RET_OP(m_op, m_data, FLOAT, _float, float);                   \
	}
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                           \
switch (p_other.type) {                                                                                                             \
	case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f));  \
	case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i));  \
}                                                                                                                                   \
break;

bool var::operator==(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, ==) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, ==) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, ==) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string == p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, ==) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, ==) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, ==) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, ==) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr == p_other.operator Array();
			}
			break;
		}
		case DICTIONARY: {
			if (p_other.type == DICTIONARY) {
				return _data._dict == p_other.operator Dictionary();
			}
			break;
		}
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj == p_other._data._obj;
			break;
		}
	}
	return false;
}

bool var::operator!=(const var& p_other) const {
	return !operator==(p_other);
}

bool var::operator<(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, <) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, <) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, <) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, < ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, < ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, < ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, < ) }
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() < *p_other.operator Array().get_data();
			break;
		}
		case DICTIONARY:
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj < p_other._data._obj;
			break;
		}
	}
	VAR_ERR("invalid < comparison");
	return false;
}

bool var::operator>(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, > ) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, > ) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, > ) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, > ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, > ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, > ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, > ) }
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() > *p_other.operator Array().get_data();
			break;
		}
		case DICTIONARY:
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj > p_other._data._obj;
			break;
		}
	}
	VAR_ERR("invalid > comparison");
	return false;
}

bool var::operator<=(const var& p_other) const {
	return *this == p_other || *this < p_other;
}
bool var::operator>=(const var& p_other) const {
	return *this == p_other || *this > p_other;
}

var var::operator +(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, + ) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, + ) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, + ) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string + p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, + ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, + ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, + ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, + ) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr + p_other._data._arr;
			}
			break;
		}
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator +");
	return false;
}

var var::operator-(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, -) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, -) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, -) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, -) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, -) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, -) }
		case ARRAY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator -");
	return false;
}

var var::operator *(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, *) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, *) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, *) }
		case STRING:
			break; // TODO: maybe python like
		case VECT2F: { VAR_SWITCH_VECT(2, f, *) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, *) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, *) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, *) }
		case ARRAY:
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator *");
	return false;
}

#define CASE_DIV_DATA(m_data1, m_data2)                                   \
{                                                                         \
if (m_data2 == 0)                                                         \
	VAR_ERR("zero division error");                                       \
return m_data1 / m_data2;                                                 \
}

#define SWITCH_DIV_TYPES(m_data, m_cast)                                  \
switch (p_other.type) {                                                   \
	case BOOL:                                                            \
		CASE_DIV_DATA(m_cast _data.m_data, (int)p_other._data._bool)      \
	case INT:                                                             \
		CASE_DIV_DATA(m_cast _data.m_data, p_other._data._int)            \
	case FLOAT:                                                           \
		CASE_DIV_DATA(m_cast _data.m_data, p_other._data._float)          \
}

var var::operator /(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { SWITCH_DIV_TYPES(_bool, (int)) }
		case INT: { SWITCH_DIV_TYPES(_int, +) }
		case FLOAT: { SWITCH_DIV_TYPES(_float , +) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, /) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, /) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, /) }
		case ARRAY:
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator /");
	return false;
}
#undef SWITCH_DIV_TYPES
#undef CASE_DIV_DATA

var var::operator %(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: {
			switch (p_other.type) {
				case BOOL: return (int)(_data._bool) % (int)(p_other._data._bool);
				case INT: return (int)_data._bool % p_other._data._int;
			}
		}
		case INT: {
			switch (p_other.type) {
				case BOOL: return _data._int % (int)(p_other._data._bool);
				case INT: return _data._int % p_other._data._int;
			}
		}
	}
	VAR_ERR("unsupported operator %");
	return false;
}
#undef VAR_RET_EQUAL
#undef VAR_SWITCH_PRIME_TYPES
#undef VAR_SWITCH_VECT
#undef SWITCH_DIV_TYPES
#undef CASE_DIV_DATA


#define VAR_CASE_OP(m_op, m_data1, m_type, m_data2, m_cast) \
	case m_type: _data.m_data1 m_op (m_cast)p_other._data.m_data2; return *this
#define VAR_SWITCH_PRIME_TYPES(m_data, m_cast, m_op)                               \
	switch (p_other.type) {                                                        \
		VAR_CASE_OP(m_op, m_data, BOOL, _bool , m_cast);                           \
		VAR_CASE_OP(m_op, m_data, INT, _int, m_cast);                              \
		VAR_CASE_OP(m_op, m_data, FLOAT, _float, m_cast);                          \
	}
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                               \
switch (p_other.type) {                                                                                                                 \
	case D_VEC(VECT, m_dim, F): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f)); return *this;     \
	case D_VEC(VECT, m_dim, I): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i)); return *this;     \
}                                                                                                                                       \
break;
var& var::operator+=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, int, +=) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, int, +=) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, +=) }
		case STRING: {
			if (p_other.type == STRING) {
				_data._string += p_other._data._string;
				return *this;
			}
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, +=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, +=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, +=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, +=) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				_data._arr += p_other._data._arr;
				return *this;
			}
			break;
		}
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator +=");
	return *this;
}

var& var::operator-=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL : { VAR_SWITCH_PRIME_TYPES(_bool, int, -=) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, int, -=) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, -=) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, -=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, -=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, -=) }
		case ARRAY:
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator -=");
	return *this;
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, int, *=) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, int, *=) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, *=) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, *=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, *=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, *=) }
		case ARRAY:
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator *=");
	return *this;
}

#define CASE_DIV_DATA(m_data1, m_data2)                                   \
{                                                                         \
if (m_data2 == 0)                                                         \
	VAR_ERR("zero division error");                                       \
m_data1 /= m_data2;                                                       \
return *this;                                                             \
}

#define SWITCH_DIV_TYPES(m_data, m_cast)                                  \
switch (p_other.type) {                                                   \
	case BOOL:                                                            \
		CASE_DIV_DATA(_data.m_data, (int)p_other._data._bool)             \
	case INT:                                                             \
		CASE_DIV_DATA(_data.m_data, (m_cast)p_other._data._int)           \
	case FLOAT:                                                           \
		CASE_DIV_DATA(_data.m_data, (m_cast)p_other._data._float)         \
}
var& var::operator/=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: { SWITCH_DIV_TYPES(_bool, bool) }
		case INT: { SWITCH_DIV_TYPES(_int, int) }
		case FLOAT: { SWITCH_DIV_TYPES(_float, float) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, /=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, /=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, /=) }
		case ARRAY:
		case DICTIONARY:
		case OBJECT:
			break;
	}
	VAR_ERR("unsupported operator /=");
	return *this;
}

var& var::operator %=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: break;
		case INT: {
			switch (p_other.type) {
				//case BOOL: _data._int %= (int)(p_other._data._bool); return *this;
				case INT: _data._int %= p_other._data._int; return *this;
			}
		}
	}
	VAR_ERR("unsupported operator %=");
	return *this;
}

void var::copy_data(const var& p_other) {
	clear_data();
	type = p_other.type;
	switch (p_other.type) {
		case var::_NULL: break;
		case var::BOOL:
			_data._bool = p_other._data._bool;
			break;
		case var::INT:
			_data._int = p_other._data._int;
			break;
		case var::FLOAT:
			_data._float = p_other._data._float;
			break;
		case var::STRING:
			new(&_data._string) String(p_other._data._string);
			break;
		case var::VECT2F:
		case var::VECT2I:
		case var::VECT3F:
		case var::VECT3I:
			for (int i = 0; i < DATA_MEM_SIZE; i++) {
				_data._mem[i] = p_other._data._mem[i];
			}
			break;
		case var::ARRAY:
			_data._arr = p_other._data._arr;
			break;
		case var::DICTIONARY:
			_data._dict = p_other._data._dict;
			break;
		case var::OBJECT:
			_data._obj = p_other._data._obj;
			return;
	}
}

void var::clear_data() {
	switch (type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
		case var::VECT2F:
		case var::VECT2I:
		case var::VECT3F:
		case var::VECT3I:
			return;
		case var::STRING:
			_data._string.~String();
			return;
		case var::ARRAY:
			_data._arr._data = nullptr;
			break;
		case var::DICTIONARY:
			_data._dict._data = nullptr;
			break;
		case var::OBJECT:
			_data._obj = nullptr;
			break;
	}
}

}

#undef VAR_CASE_OP
#undef VAR_SWITCH_PRIME_TYPES
#undef VAR_SWITCH_VECT
#undef CASE_DIV_DATA
#undef SWITCH_DIV_TYPES

#if defined(UNDEF_VAR_DEFINES)
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef newptr
#undef newptr2
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_BREAK
#undef VAR_ERR
#undef VAR_ASSERT
#undef UNDEF_VAR_DEFINES
#endif
