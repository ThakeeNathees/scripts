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
	p_ostream << p_str.operator std::string();
	return p_ostream;
}
std::istream& operator>>(std::istream& p_istream, String& p_str) {
	p_istream >> p_str._data;
	return p_istream;
}

std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.to_string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr) {
	p_ostream << p_arr.to_string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Map& p_map) {
	p_ostream << p_map.to_string();
	return p_ostream;
}

// String -----------------------------------------------
var String::call_method(const String& p_method, const stdvec<var>& p_args) {
	if (p_method == "to_int") {
		// TODO: check args.size() == 0
		return to_int();
	} else if (p_method == "to_float") {
		// TODO: check args.size() == 0
		return to_float();
	} else if (p_method == "get_line") {
		// TODO: check args.size() == 1, type == INT.
		return get_line(p_args[0].operator int64_t());
	} else if (p_method == "hash") {
		// TODO: check args.size() == 0
		return (int64_t)hash();
	} else if (p_method == "substr") {
		// TODO: check args.size() == 2, type == INT.
		return substr((size_t)p_args[0].operator int64_t(), (size_t)p_args[1].operator int64_t());
	} else if (p_method == "endswith") {
		// TODO: check args.size() == 1, type == STRING
		return endswith(p_args[0].operator String());
	} else if (p_method == "startswith") {
		// TODO: check args.size() == 1, type == STRING
		return startswith(p_args[0].operator String());
	}
	// TODO: more.
	throw VarError(VarError::INVALID_GET_NAME, ""); // TODO:
}

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

String String::get_line(uint64_t p_line) const {
	const char* source = _data.c_str();
	uint64_t cur_line = 1;
	std::stringstream ss_line;

	while (char c = *source) {
		if (c == '\n') {
			if (cur_line >= p_line) break;
			cur_line++;
		} else if (cur_line == p_line) {
			ss_line << c;
		}
		source++;
	}

	ss_line << '\n';
	return ss_line.str();
}

String String::substr(size_t p_start, size_t p_end) const {
	return _data.substr(p_start, p_end - p_start);
}
bool String::endswith(const String& p_str) const {
	if (p_str.size() > _data.size()) return false;
	for (size_t i = 1; i <= p_str.size(); i++) {
		if (_data[_data.size() - i] != p_str[p_str.size() - i]) {
			return false;
		}
	}
	return true;
}
bool String::startswith(const String& p_str) const {
	if (p_str.size() > _data.size()) return false;
	for (size_t i = 0; i < p_str.size(); i++) {
		if (_data[i] != p_str[i]) {
			return false;
		}
	}
	return true;
}

bool operator==(const char* p_cstr, const String& p_str) {
	return p_str == String(p_cstr);
}
bool operator!=(const char* p_cstr, const String& p_str) {
	return p_str != String(p_cstr);
}

// Array -----------------------------------------------

String Array::to_string() const {
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

// Map  ----------------------------------------
String Map::to_string() const {
	std::stringstream ss;
	ss << "{ ";
	for (stdmap<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (it != (*_data).begin()) ss << ", ";
		ss << it->first.to_string() << " : " << it->second.to_string();
	}
	ss << " }";
	return ss.str();
}

Map Map::copy(bool p_deep) const {
	Map ret;
	for (stdmap<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (p_deep)
			ret[it->first] = it->second.copy(true);
		else
			ret[it->first] = it->second;
	}
	return ret;
}

var& Map::operator[](const var& p_key) const { return (*_data).operator[](p_key); }
var& Map::operator[](const var& p_key) { return (*_data).operator[](p_key);}
var& Map::operator[](const char* p_key) const { return (*_data).operator[](p_key); }
var& Map::operator[](const char* p_key) { return (*_data).operator[](p_key); }

stdmap<var, var>::iterator Map::begin() const { return (*_data).begin(); }
stdmap<var, var>::iterator Map::end() const { return (*_data).end(); }
stdmap<var, var>::iterator Map::find(const var& p_key) const { return (*_data).find(p_key); }

void Map::insert(const var& p_key, const var& p_value) { (*_data).insert(std::pair<var, var>(p_key, p_value)); }
bool Map::has(const var& p_key) const { return find(p_key) != end(); }

bool Map::operator ==(const Map& p_other) const {
	if (size() != p_other.size())
		return false;
	for (stdmap<var, var>::iterator it_other = p_other.begin(); it_other != p_other.end(); it_other++) {
		stdmap<var, var>::iterator it_self = find(it_other->first);
		if (it_self == end()) return false;
		if (it_self->second != it_other->second) return false;

	}
	return true;
}

Map& Map::operator=(const Map& p_other) {
	_data = p_other._data;
	return *this;
}

// Object -----------------------------------------------

var Object::operator+(const var& p_other) const { return __add(p_other); }
var Object::operator-(const var& p_other) const { return __sub(p_other); }
var Object::operator*(const var& p_other) const { return __mul(p_other); }
var Object::operator/(const var& p_other) const { return __div(p_other); }

var& Object::operator+=(const var& p_other) { return __add_eq(p_other); }
var& Object::operator-=(const var& p_other) { return __sub_eq(p_other); }
var& Object::operator*=(const var& p_other) { return __mul_eq(p_other); }
var& Object::operator/=(const var& p_other) { return __div_eq(p_other); }

var Object::operator[](const var& p_key) const { return __get_mapped(p_key); }
var& Object::operator[](const var& p_key) { throw VarError(VarError::NOT_IMPLEMENTED); }

#ifndef _VAR_H_EXTERN_IMPLEMENTATIONS
// call_method() should call it's parent if method not exists.
var Object::call_method(ptr<Object> p_self, const String& p_name, stdvec<var>& p_args) { throw VarError(VarError::INVALID_GET_NAME); }
#endif

var Object::__call(stdvec<var>& p_vars) { throw VarError(VarError::NOT_IMPLEMENTED); }
var Object::operator()(stdvec<var>& p_vars) { return __call(p_vars); }

bool Object::__has(const String& p_name) const { return false; }
var& Object::__get(const String& p_name) { throw VarError(VarError::INVALID_GET_NAME, String::format("Name \"%s\" not exists in object.", p_name)); }
//void Object::__set(const String& p_name, const var& p_val) { throw VarError(VarError::INVALID_SET_NAME, String::format("Name \"%s\" not exists in object.", p_name)); }

bool Object::__has_mapped(const String& p_name) const { return false; }
var Object::__get_mapped(const var& p_key) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
void Object::__set_mapped(const var& p_key, const var& p_val) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

var Object::__add(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var Object::__sub(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var Object::__mul(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var Object::__div(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

var& Object::__add_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var& Object::__sub_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var& Object::__mul_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var& Object::__div_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

bool Object::__gt(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); } // TODO: This will throw if
bool Object::__lt(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); } // object used as key in a Map.
bool Object::__eq(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

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
		case MAP: return _data._map.copy(p_deep);
		case OBJECT: return _data._obj->copy(p_deep);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
			// TODO:
	return *this;
}

const char* var::get_class_name() const {
	switch (type) {
		case var::_NULL:  return "null";
		case var::BOOL:   return "bool";
		case var::INT:    return "int";
		case var::FLOAT:  return "float";
		case var::STRING: return "String";
		case var::VECT2F: return "Vect2f";
		case var::VECT2I: return "Vect2i";
		case var::VECT3F: return "Vect3f";
		case var::VECT3I: return "Vect3i";
		case var::ARRAY:  return "Array";
		case var::MAP:    return "Map";
		case var::OBJECT: return _data._obj->get_class_name();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return nullptr;
}

const char* var::get_parent_class_name() const {
	switch (type) {
		case var::OBJECT:
			return _data._obj->get_parent_class_name();
		default:
			return nullptr;
	}
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

var::var(int64_t p_int) {
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

var::var(const Map& p_map) {
	type = MAP;
	_data._map = p_map;
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)                              \
var var::operator m_op () {                                     \
	switch (type) {                                             \
		case INT:  return m_op _data._int;                      \
		case FLOAT: return m_op _data._float;                   \
		default: throw VarError(VarError::INVALID_CASTING, ""); \
	}                                                           \
	return var();                                               \
}

#define VAR_OP_POST_INCR_DECR(m_op)                             \
var var::operator m_op(int) {                                   \
	switch (type) {                                             \
		case INT: return _data._int m_op;                       \
		case FLOAT: return _data._float m_op;                   \
		default: throw VarError(VarError::INVALID_CASTING, ""); \
	}                                                           \
	return var();                                               \
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

var& var::operator[](const var& p_key) const {
	switch (type) {
		// strings can't return char as var&
		case ARRAY: {
			int index = (int)p_key;
			if (0 <= index && index < (int)_data._arr.size())
				return _data._arr[index];
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				return _data._arr[_data._arr.size() + index];
			throw VarError(VarError::INVALID_INDEX, "");
		}
		case MAP:
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->operator [](p_key);
	}
	throw VarError(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
	return var::tmp;
}

var var::__get_mapped(const var& p_key) const {
	switch (type) {
		case STRING: {
			int index = (int)p_key; // TODO: check key is int.
			if (0 <= index && index < (int)_data._string.size())
				return String(_data._string[index]);
			if ((int)_data._string.size() * -1 <= index && index < 0)
				return String(_data._string[_data._string.size() + index]);
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case ARRAY: {
			int index = (int)p_key; // TODO: check key is int.
			if (0 <= index && index < (int)_data._arr.size())
				return _data._arr[index];
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				return _data._arr[_data._arr.size() + index];
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case MAP:
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->__get_mapped(p_key);
	}
	throw VarError(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
	return var();
}
void var::__set_mapped(const var& p_key, const var& p_value) {
	switch (type) {
		case STRING: {
			int index = (int)p_key; // TODO: check key is int.
			// TODO: check value is string, size = 1.
			if (0 <= index && index < (int)_data._string.size())
				_data._string[index] = p_value.operator String()[0];
			if ((int)_data._string.size() * -1 <= index && index < 0)
				_data._string[_data._string.size() + index] = p_value.operator String()[0];
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case ARRAY: {
			int index = (int)p_key; // TODO: check key is int.
			if (0 <= index && index < (int)_data._arr.size())
				_data._arr[index] = p_value;
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				_data._arr[_data._arr.size() + index] = p_value;
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case MAP:
			_data._map[p_key] = p_value;
		case OBJECT:
			return _data._obj->__set_mapped(p_key, p_value);
	}
	throw VarError(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
}

#define VECT2_GET(m_t)                                            \
	if (p_name == "x" || p_name == "width") {                     \
		return (*DATA_PTR_CONST(STRCAT2(Vect2, m_t))).x;          \
	} else if (p_name == "y" || p_name == "height") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect2, m_t))).y;          \
	} else throw VarError(VarError::INVALID_GET_NAME)

#define VECT3_GET(m_t)                                            \
	if (p_name == "x" || p_name == "width") {                     \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).x;          \
	} else if (p_name == "y" || p_name == "height") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).y;          \
	} else if (p_name == "z" || p_name == "depth") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).y;          \
	} else throw VarError(VarError::INVALID_GET_NAME)

#define VECT2_SET(m_t, m_cast)                                             \
	if (p_name == "x" || p_name == "width") {                              \
		(*DATA_PTR(STRCAT2(Vect2, m_t))).x = p_value.operator m_cast();    \
	} else if (p_name == "y" || p_name == "height") {			           \
		(*DATA_PTR(STRCAT2(Vect2, m_t))).y = p_value.operator m_cast();    \
	} else throw VarError(VarError::INVALID_GET_NAME)

#define VECT3_SET(m_t, m_cast)                                             \
	if (p_name == "x" || p_name == "width") {                              \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).x = p_value.operator m_cast();    \
	} else if (p_name == "y" || p_name == "height") {			           \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).y = p_value.operator m_cast();    \
	} else if (p_name == "z" || p_name == "depth") {			           \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).y = p_value.operator m_cast();    \
	} else throw VarError(VarError::INVALID_GET_NAME)



var var::__get(const String& p_name) const {
	switch (type) {
		case VECT2F: VECT2_GET(f);
		case VECT2I: VECT2_GET(i);
		case VECT3F: VECT3_GET(f);
		case VECT3I: VECT3_GET(i);
		case OBJECT: return _data._obj->__get(p_name);
	}
	throw VarError(VarError::INVALID_GET_NAME); // TODO: more clear error.
	return var::tmp;
}

void var::__set(const String& p_name, const var& p_value) {
	switch (type) {
		case VECT2F: 
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT2_SET(f, double);
			return;
		case VECT2I:
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT2_SET(i, int64_t);
			return;
		case VECT3F:
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT3_SET(f, double);
			return;
		case VECT3I:
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT3_SET(i, int64_t);
			return;
		case OBJECT: 
			_data._obj->__get(p_name) = p_value;
			return;
	}
	throw VarError(VarError::INVALID_GET_NAME); // TODO: more clear error.
}

#undef VECT2_GET
#undef VECT3_GET

var var::__call_internal(stdvec<var>& p_args) {
	switch (type) {
		case var::_NULL:  throw VarError(VarError::NULL_POINTER, "");
		case var::BOOL:   throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not callable");
		case var::INT:    throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "integer is not callable");
		case var::FLOAT:  throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "float is not callable");
		case var::STRING: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "String is not callable");
		case var::VECT2F: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT2I: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT3F: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT3I: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::ARRAY:  throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Array is not callable");
		case var::MAP:    throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Map is not callable");
		case var::OBJECT: return _data._obj->__call(p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return var();
}
var var::call_method_internal(const String& p_method, stdvec<var>& p_args) {
	switch (type) {
		case var::_NULL:  throw VarError(VarError::NULL_POINTER, "");
		case var::BOOL:   throw VarError(VarError::INVALID_GET_NAME, String::format("boolean has no attribute \"%s\"", p_method.c_str()));
		case var::INT:    throw VarError(VarError::INVALID_GET_NAME, String::format("int has no attribute \"%s\"", p_method.c_str()));
		case var::FLOAT:  throw VarError(VarError::INVALID_GET_NAME, String::format("float has no attribute \"%s\"", p_method.c_str()));
		case var::STRING: return _data._string.call_method(p_method, p_args);
		case var::VECT2F: // TODO:
		case var::VECT2I: // TODO:
		case var::VECT3F: // TODO:
		case var::VECT3I: // TODO:
		case var::ARRAY:  // TODO:
		case var::MAP:    // TODO:
		case var::OBJECT: return Object::call_method(_data._obj, p_method, p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return var();
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
		case MAP: return !_data._map.empty();
		case OBJECT: return _data._obj.operator bool();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"BOOL\"", get_type_name(type).c_str()));
	return false;
}

var::operator int64_t() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		//case STRING: return  _data._string.to_int(); // throws std::exception
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"INT\"", get_type_name(type).c_str()));
}

var::operator double() const {
	switch (type) {
		case BOOL: return (double)_data._bool;
		case INT: return (double)_data._int;
		case FLOAT: return _data._float;
		//case STRING: return  _data._string.to_float();
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"FLOAT\"", get_type_name(type).c_str()));
}

var::operator String() const {
	if (type != STRING)
		throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"STRING\"", get_type_name(type).c_str()));
	return _data._string;
}

String var::to_string() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return String(_data._int);
		case FLOAT: return String(_data._float);
		case STRING: return _data._string;
		case VECT2F: return (*DATA_PTR_CONST(Vect2f)).to_string();
		case VECT2I: return (*DATA_PTR_CONST(Vect2i)).to_string();
		case VECT3F: return (*DATA_PTR_CONST(Vect3f)).to_string();
		case VECT3I: return (*DATA_PTR_CONST(Vect3i)).to_string();
		case ARRAY: return _data._arr.to_string();
		case MAP: return _data._map.to_string();
		case OBJECT: return _data._obj->to_string();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	DEBUG_BREAK(); // can't be reach here.
	return "";
}

#define VAR_VECT_CAST(m_dim, m_t)                                                       \
var::operator D_VEC(Vect, m_dim, m_t)() const {                                         \
	switch (type) {                                                                     \
		case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, f));      \
		case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, i));      \
		default: throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"" STRINGIFY(D_VEC(VECT, m_dim, I)) "\"", get_type_name(type).c_str()));\
	}                                                                                   \
	return D_VEC(Vect, m_dim, m_t)();                                                   \
}
VAR_VECT_CAST(2, f)
VAR_VECT_CAST(2, i)
VAR_VECT_CAST(3, f)
VAR_VECT_CAST(3, i)
#undef VAR_VECT_CAST

var::operator Array() const {
	if (type == ARRAY) {
		return _data._arr;
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"ARRAY\"", get_type_name(type).c_str()));
}

var::operator Map() const {
	if (type == MAP) {
		return _data._map;
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"MAP\"", get_type_name(type).c_str()));
}

var::operator ptr<Object>() const {
	if (type == OBJECT) {
		return _data._obj;
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"OBJECT\"", get_type_name(type).c_str()));
}

/* operator overloading */
		/* comparison */
#define VAR_RET_OP(m_op, m_data1, m_type, m_data2, m_cast) \
	case m_type: return (m_cast)_data.m_data1 m_op p_other._data.m_data2
#define VAR_SWITCH_PRIME_TYPES(m_data, m_op)                              \
	switch (p_other.type) {                                               \
		VAR_RET_OP(m_op, m_data, BOOL, _bool, bool);                      \
		VAR_RET_OP(m_op, m_data, INT, _int, int64_t);                     \
		VAR_RET_OP(m_op, m_data, FLOAT, _float, float);                   \
	}
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                           \
switch (p_other.type) {                                                                                                             \
	case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f));  \
	case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i));  \
}                                                                                                                                   \
break;

#define THROW_OPERATOR_NOT_SUPPORTED(m_op)                                                                                                \
do {																																	  \
	String operand1, operand2;																											  \
	if (type != OBJECT) operand1 = get_type_name(type);																					  \
	else operand1 = _data._obj->get_class_name();																						  \
	if (p_other.get_type() != OBJECT) operand2 = get_type_name(p_other.get_type());														  \
	else operand2 = p_other._data._obj->get_class_name();																				  \
	throw VarError(VarError::OPERATOR_NOT_SUPPORTED,																					  \
		String::format("operator \"" STR(m_op) "\" not supported on operands \"%s\" and \"%s\"", operand1.c_str(), operand2.c_str())	  \
	);																																	  \
} while(false)

bool var::operator==(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:   { VAR_SWITCH_PRIME_TYPES(_bool, ==)  } break;
		case INT:    { VAR_SWITCH_PRIME_TYPES(_int, ==)   } break;
		case FLOAT:  { VAR_SWITCH_PRIME_TYPES(_float, ==) } break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string == p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, ==) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, ==) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, ==) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, ==) } break;
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr == p_other.operator Array();
			}
			break;
		}
		case MAP: {
			if (p_other.type == MAP) {
				return _data._map == p_other.operator Map();
			}
			break;
		}
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj == p_other._data._obj;
			break;
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return false;
}

bool var::operator!=(const var& p_other) const {
	return !operator==(p_other);
}

bool var::operator<(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  { VAR_SWITCH_PRIME_TYPES(_bool, <)  } break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, <)   } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, <) } break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, < ) }  break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, < ) }  break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, < ) }  break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, < ) }  break;
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() < *p_other.operator Array().get_data();
			break;
		}
		case MAP: {
			break;
		}
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj < p_other._data._obj;
			break;
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	// FIXME: a workaround for map keys as vars.
	return this < &p_other;
}

bool var::operator>(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  { VAR_SWITCH_PRIME_TYPES(_bool, > )  } break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, > )   } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, > ) } break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, > ) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, > ) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, > ) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, > ) } break;
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() > *p_other.operator Array().get_data();
			break;
		}
		case MAP:
			break;
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj > p_other._data._obj;
			break;
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return this < &p_other;
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
		case BOOL:  { VAR_SWITCH_PRIME_TYPES(_bool, + )  } break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, + )   } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, + ) } break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string + p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, + ) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, + ) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, + ) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, + ) } break;
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr + p_other._data._arr;
			}
			break;
		}
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(+);
}

var var::operator-(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  { VAR_SWITCH_PRIME_TYPES(_bool, -)  } break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, -)   } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, -) } break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, -) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, -) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, -) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(-);
}

var var::operator *(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  { VAR_SWITCH_PRIME_TYPES(_bool, *)  } break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, *)   } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, *) } break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, *) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, *) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, *) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(*);
}

#define CASE_DIV_DATA(m_data1, m_data2)                                   \
{                                                                         \
if (m_data2 == 0)                                                         \
	throw VarError(VarError::ZERO_DIVISION, "");                          \
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
		case BOOL:  { SWITCH_DIV_TYPES(_bool, (int)) } break;
		case INT:   { SWITCH_DIV_TYPES(_int, +)      } break;
		case FLOAT: { SWITCH_DIV_TYPES(_float , +)   } break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, /) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, /) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, /) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(/);
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(%);
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
		case _NULL:
		case BOOL:
			break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, int, +=)     } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, +=) } break;
		case STRING: {
			if (p_other.type == STRING) {
				_data._string += p_other._data._string;
				return *this;
			}
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, +=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, +=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, +=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, +=) } break;
		case ARRAY: {
			if (p_other.type == ARRAY) {
				_data._arr += p_other._data._arr;
				return *this;
			}
			break;
		}
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(+=);
}

var& var::operator-=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL:
			break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, int, -=)     } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, -=) } break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, -=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, -=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, -=) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(-=);
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL:
			break;
		case INT:   { VAR_SWITCH_PRIME_TYPES(_int, int, *=)     } break;
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, *=) } break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, *=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, *=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, *=) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(*=);
}

#define CASE_DIV_DATA(m_data1, m_data2)                                   \
{                                                                         \
if (m_data2 == 0)                                                         \
	throw VarError(VarError::ZERO_DIVISION, "");                          \
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
		case _NULL:
		case BOOL:
			break;
		case INT:   { SWITCH_DIV_TYPES(_int, int)     } break;
		case FLOAT: { SWITCH_DIV_TYPES(_float, float) } break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, /=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, /=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, /=) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(/=);
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
	THROW_OPERATOR_NOT_SUPPORTED(%=);
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
		case var::MAP:
			_data._map = p_other._data._map;
			break;
		case var::OBJECT:
			_data._obj = p_other._data._obj;
			return;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
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
		case var::MAP:
			_data._map._data = nullptr;
			break;
		case var::OBJECT:
			_data._obj = nullptr;
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
}


String var::get_type_name(var::Type p_type) {
	static const char* _type_names[_TYPE_MAX_] = {
		"null",
		"bool",
		"int",
		"float",
		"String",
		"Vect2f",
		"Vect2i",
		"Vect3f",
		"Vect3i",
		"Array",
		"Map",
		"Object",
		//_TYPE_MAX_
	};
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return _type_names[p_type];
}

}

#undef VAR_CASE_OP
#undef VAR_SWITCH_PRIME_TYPES
#undef VAR_SWITCH_VECT
#undef CASE_DIV_DATA
#undef SWITCH_DIV_TYPES

#if defined(UNDEF_VAR_DEFINES)
#undef func
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_BREAK
#undef DEBUG_PRINT
#undef VAR_ASSERT
#undef MISSED_ENUM_CHECK
#undef UNDEF_VAR_DEFINES
#endif
