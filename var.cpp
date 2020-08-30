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
#include <functional>

// add __FUNCTION__, __LINE__, __FILE__ to VarError.
#define THROW_ERROR(m_type, m_msg) \
	throw VarError(m_type, m_msg)

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

static const char* _error_names[VarError::_ERROR_MAX_] = {
	"OK",
	"NULL_POINTER",
	"INVALID_INDEX",
	"INVALID_KEY",
	"INVALID_CASTING",
	"INVALID_GET_NAME",
	"INVALID_SET_VALUE",
	"NOT_IMPLEMENTED",
	"OPERATOR_NOT_SUPPORTED",
	"ZERO_DIVISION",
	"BUG",
	//_ERROR_MAX_
};
MISSED_ENUM_CHECK(VarError::_ERROR_MAX_, 13);

std::string VarError::get_err_name(VarError::Type p_type) {
	return _error_names[p_type];
}

#define CHECK_METHOD_AND_ARGS()                                                                                                                         \
do {                                                                                                                                                    \
	if (has_member(p_method)) {																															\
		if (get_member_info(p_method)->get_type() != MemberInfo::METHOD)																						\
			THROW_ERROR(VarError::INVALID_GET_NAME, String::format("member \"%s\" is not callable.", p_method.c_str()));								\
		const MethodInfo* mp = (MethodInfo*)get_member_info(p_method);																						\
		int arg_count = mp->get_arg_count();																											\
		int default_arg_count = mp->get_default_arg_count();																							\
		if (arg_count != -1) {																															\
			if (p_args.size() + default_arg_count < arg_count) { /* Args not enough. */																    \
				if (default_arg_count == 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, String::format("expected at exactly %i argument(s).", arg_count));	\
				else THROW_ERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", arg_count - default_arg_count));		\
			} else if (p_args.size() > arg_count) { /* More args proveded.	*/																			\
				if (default_arg_count == 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, String::format("expected at exactly %i argument(s).", arg_count));	\
				else THROW_ERROR(VarError::INVALID_ARG_COUNT, String::format(																			\
					"expected minimum of %i argument(s) and maximum of %i argument(s).", arg_count - default_arg_count, arg_count));					\
			}																																			\
		}																																				\
		for (int j = 0; j < mp->get_arg_types().size(); j++) {																							\
			if (mp->get_arg_types()[j] == VarTypeInfo(var::VAR)) continue; /* can't be _NULL. */														\
			if (p_args.size() == j) break; /* rest are default args. */																					\
			if (mp->get_arg_types()[j] != VarTypeInfo(p_args[j].get_type(), p_args[j].get_type_name().c_str()))								            \
				THROW_ERROR(VarError::INVALID_ARGUMENT, String::format(																					\
					"expected type \"%s\" at argument %i.", var::get_type_name_s(mp->get_arg_types()[j].type), j));										\
		}																																				\
	} else {																																			\
		THROW_ERROR(VarError::INVALID_GET_NAME, ""); /* TODO: more clear message. */																	\
	}																																					\
} while (false)

#define MEMBER_INFO_IMPLEMENTATION(m_type)                                                                                                              \
bool m_type::has_member(const String& p_member) {																										\
	return get_member_info_list().find(p_member) != get_member_info_list().end();																					\
}																																						\
const MemberInfo* m_type::get_member_info(const String& p_member) {																							\
	if (!has_member(p_member)) THROW_ERROR(VarError::INVALID_KEY, String::format("member \"%s\" not exists on base " #m_type , p_member.c_str()));		\
	const stdmap<String, const MemberInfo*>& member_info = get_member_info_list();																			\
	return member_info.at(p_member);																													\
}																																						\
const stdmap<String, const MemberInfo*>& m_type::get_member_info_list()

// String -----------------------------------------------

MEMBER_INFO_IMPLEMENTATION(String) {
	static const stdmap<String, const MemberInfo*> member_info = {
		{ "size",           new MethodInfo("size", var::INT),								                },
		{ "length",         new MethodInfo("length", var::INT),								                },
		{ "to_int",         new MethodInfo("to_int", var::INT),								                },
		{ "to_float",       new MethodInfo("to_float", var::FLOAT),							                },
		{ "get_line",       new MethodInfo("get_line", {"line"}, {var::INT}, var::STRING),                  },
		{ "hash",           new MethodInfo("hash", var::INT),							                    },
		{ "substr",         new MethodInfo("substr", {"start", "end"}, {var::INT, var::INT}, var::STRING),	},
		{ "endswith",       new MethodInfo("endswith", {"what"}, {var::STRING}),							},
		{ "startswith",     new MethodInfo("startswith", {"what"}, {var::STRING}),                          },
	};
	return member_info;
}

var String::call_method(const String& p_method, const stdvec<var>& p_args) {

	CHECK_METHOD_AND_ARGS();

	switch (p_method.const_hash()) {
		case "size"_hash:        // [[fallthrough]]
		case "length"_hash:      return (int64_t)size();
		case "to_int"_hash :     return to_int();
		case "to_float"_hash :   return to_float();
		case "get_line"_hash :   return get_line(p_args[0].operator int64_t());
		case "hash"_hash :       return (int64_t)hash();
		case "substr"_hash :     return substr((size_t)p_args[0].operator int64_t(), (size_t)p_args[1].operator int64_t());
		case "endswith"_hash :   return endswith(p_args[0].operator String());
		case "startswith"_hash : return startswith(p_args[0].operator String());
	}
	// TODO: more.
	THROW_ERROR(VarError::BUG, "!!! BUG !!! It can't reach here.");
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

MEMBER_INFO_IMPLEMENTATION(Array) {
	static const stdmap<String, const MemberInfo*> member_info = {
		{ "size",      new MethodInfo("size", var::INT),                                     },
		{ "empty",     new MethodInfo("empty", var::BOOL),                                   },
		{ "push_back", new MethodInfo("push_back", {"element"}, {var::VAR}, var::_NULL),     },
		{ "pop_back",  new MethodInfo("pop_back", var::_NULL),                               },
		{ "append",    new MethodInfo("append", {"element"}, {var::VAR}, var::ARRAY),        },
		{ "pop",       new MethodInfo("pop", var::VAR),                                      },
		{ "clear",     new MethodInfo("clear", var::_NULL),                                  },
		{ "at",        new MethodInfo("at", {"index"}, {var::INT}, var::VAR),                },
	};
	return member_info;
}

var Array::call_method(const String& p_method, const stdvec<var>& p_args) {
	CHECK_METHOD_AND_ARGS();
	switch (p_method.const_hash()) {
		case "size"_hash:      return (int64_t)size();
		case "empty"_hash:     return empty();
		case "push_back"_hash: { push_back(p_args[0]); return var(); }
		case "pop_back"_hash:  { pop_back(); return var(); }
		case "append"_hash:    return append(p_args[0]);
		case "pop"_hash:       return pop();
		case "clear"_hash:     { clear(); return var(); }
		case "at"_hash:        return at(p_args[0].operator int64_t());
	}
	// TODO: add more.
	THROW_ERROR(VarError::BUG, "!!! BUG !!! It can't reach here.");
}

String Array::to_string() const {
	// TODO: if the array contains itself it'll stack overflow.
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

var Array::pop(){ var ret = this->operator[](size() - 1); pop_back(); return ret; }

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


MEMBER_INFO_IMPLEMENTATION(Map) {
	static const stdmap<String, const MemberInfo*> member_info = {
		//{ "to_int",         new MethodInfo("to_int", var::INT),								              },
		//{ "to_float",       new MethodInfo("to_float", var::FLOAT),							              },
		//{ "get_line",       new MethodInfo("get_line", {"line"}, {var::INT}, var::STRING),                  },
		//{ "hash",           new MethodInfo("hash", var::INT),							                      },
		//{ "substr",         new MethodInfo("substr", {"start", "end"}, {var::INT, var::INT}, var::STRING),  },
		//{ "endswith",       new MethodInfo("endswith", {"what"}, {var::STRING}),							  },
		//{ "startswith",     new MethodInfo("startswith", {"what"}, {var::STRING}),                          },
	};
	return member_info;
}

var Map::call_method(const String& p_method, const stdvec<var>& p_args) {
	CHECK_METHOD_AND_ARGS();
	//switch (p_method.const_hash()) {
	//	case "to_int"_hash:     return to_int();
	//	case "to_float"_hash:   return to_float();
	//	case "get_line"_hash:   return get_line(p_args[0].operator int64_t());
	//	case "hash"_hash:       return (int64_t)hash();
	//	case "substr"_hash:     return substr((size_t)p_args[0].operator int64_t(), (size_t)p_args[1].operator int64_t());
	//	case "endswith"_hash:   return endswith(p_args[0].operator String());
	//	case "startswith"_hash: return startswith(p_args[0].operator String());
	//}
	// TODO: more.
	THROW_ERROR(VarError::BUG, "!!! BUG !!! It can't reach here.");
}

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
var& Object::operator[](const var& p_key) { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "use __set_mapped() instead"); }

#ifndef _VAR_H_EXTERN_IMPLEMENTATIONS
void Object::_bind_data() {}
// call_method() should call it's parent if method not exists.
var Object::call_method(ptr<Object> p_self, const String& p_name, stdvec<var>& p_args) { THROW_ERROR(VarError::INVALID_GET_NAME, ""); }
var Object::get_member(ptr<Object> p_self, const String& p_name) { THROW_ERROR(VarError::INVALID_GET_NAME, ""); }
void Object::set_member(ptr<Object> p_self, const String& p_name, var& p_value) { THROW_ERROR(VarError::INVALID_GET_NAME, ""); } // TODO: error name.
#endif

var Object::__call(stdvec<var>& p_vars) { THROW_ERROR(VarError::NOT_IMPLEMENTED, ""); }
var Object::operator()(stdvec<var>& p_vars) { return __call(p_vars); }

var Object::__get_mapped(const var& p_key) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
void Object::__set_mapped(const var& p_key, const var& p_val) { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }

var Object::__add(const var& p_other) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
var Object::__sub(const var& p_other) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
var Object::__mul(const var& p_other) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
var Object::__div(const var& p_other) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }

var& Object::__add_eq(const var& p_other) { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
var& Object::__sub_eq(const var& p_other) { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
var& Object::__mul_eq(const var& p_other) { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }
var& Object::__div_eq(const var& p_other) { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); }

bool Object::__gt(const var& p_other) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); } // TODO: This will throw if
bool Object::__lt(const var& p_other) const { THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, ""); } // object used as key in a Map.
bool Object::__eq(const var& p_other) const {
	if (p_other.get_type() != var::OBJECT) return false;
	return this == p_other.operator varh::ptr<varh::Object>().get();
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
		case MAP: return _data._map.copy(p_deep);
		case OBJECT: return _data._obj->copy(p_deep);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
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
		default: THROW_ERROR(VarError::INVALID_CASTING, "");    \
	}                                                           \
	return var();                                               \
}

#define VAR_OP_POST_INCR_DECR(m_op)                             \
var var::operator m_op(int) {                                   \
	switch (type) {                                             \
		case INT: return _data._int m_op;                       \
		case FLOAT: return _data._float m_op;                   \
		default: THROW_ERROR(VarError::INVALID_CASTING, "");    \
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

var var::operator[](const var& p_key) const {
	switch (type) {
		// strings can't return char as var&
		case STRING: return _data._string[p_key.operator int64_t()];
		case ARRAY:  return _data._arr[p_key.operator int64_t()];
		case MAP:    return _data._map[p_key];
		case OBJECT: return _data._obj->__get_mapped(p_key);
	}
	THROW_ERROR(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
}

var var::__get_mapped(const var& p_key) const {
	switch (type) {
		case STRING: {
			if (p_key.get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value for indexing.");
			int64_t index = p_key;
			return String(_data._string[index]);
		} break;
		case ARRAY: {
			if (p_key.get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value for indexing.");
			int64_t index = p_key;
			return _data._arr[index];
		} break;
		case MAP:
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->__get_mapped(p_key);
	}
	THROW_ERROR(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
}

void var::__set_mapped(const var& p_key, const var& p_value) {
	switch (type) {
		case STRING: {
			if (p_key.get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value for indexing.");
			if (p_value.get_type() != var::STRING) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a string value to assign");
			if (p_value.operator String().size() != 1) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a string of size 1 to assign");
			_data._string[p_key.operator int64_t()] = p_value.operator String()[0];
		} break;
		case ARRAY: {
			if (p_key.get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value for indexing.");
			_data._arr[p_key.operator int64_t()] = p_value;
		} break;
		case MAP:
			_data._map[p_key] = p_value; // TODO: refactor map.
		case OBJECT:
			_data._obj->__set_mapped(p_key, p_value);
	}
	THROW_ERROR(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
}

var var::__call_internal(stdvec<var>& p_args) {
	switch (type) {
		case var::_NULL:  THROW_ERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not callable");
		case var::INT:    THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "integer is not callable");
		case var::FLOAT:  THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "float is not callable");
		case var::STRING: THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "String is not callable");
		case var::VECT2F: THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT2I: THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT3F: THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT3I: THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::ARRAY:  THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "Array is not callable");
		case var::MAP:    THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED, "Map is not callable");
		case var::OBJECT: return _data._obj->__call(p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::BUG, "");
}

var var::call_method_internal(const String& p_method, stdvec<var>& p_args) {

	// check var methods.
	switch (p_method.const_hash()) {
		case "to_string"_hash :
			if (p_args.size() != 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
			return to_string();
		case "copy"_hash:  return copy();
		case "get_type_name"_hash: return get_type_name();

		// operators.
		case "__get_mapped"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return __get_mapped(p_args[0]);
		case "__set_mapped"_hash:
			if (p_args.size() != 2) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			__set_mapped(p_args[0], p_args[1]); return var();
		case "__add"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator +(p_args[0]);
		case "__sub"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator -(p_args[0]);
		case "__mul"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator *(p_args[0]);
		case "__div"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator /(p_args[0]);
		case "__add_eq"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator +=(p_args[0]);
		case "__sub_eq"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator -=(p_args[0]);
		case "__mul_eq"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator *=(p_args[0]);
		case "__div_eq"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator /=(p_args[0]);
		case "__gt"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator >(p_args[0]);
		case "__lt"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator <(p_args[0]);
		case "__eq"_hash:
			if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 1 argument.");
			return operator ==(p_args[0]);

		case "__call"_hash :
			return __call_internal(p_args);
	}

	// type methods.
	switch (type) {
		case var::_NULL:  THROW_ERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_ERROR(VarError::INVALID_GET_NAME, String::format("boolean has no attribute \"%s\"", p_method.c_str()));
		case var::INT:    THROW_ERROR(VarError::INVALID_GET_NAME, String::format("int has no attribute \"%s\"", p_method.c_str()));
		case var::FLOAT:  THROW_ERROR(VarError::INVALID_GET_NAME, String::format("float has no attribute \"%s\"", p_method.c_str()));
		case var::STRING: return _data._string.call_method(p_method, p_args);
		case var::VECT2F: throw "TODO"; // TODO: 
		case var::VECT2I: throw "TODO"; // TODO: 
		case var::VECT3F: throw "TODO"; // TODO: 
		case var::VECT3I: throw "TODO"; // TODO: 
		case var::ARRAY:  return _data._arr.call_method(p_method, p_args);
		case var::MAP:    return _data._map.call_method(p_method, p_args);
		case var::OBJECT: return Object::call_method(_data._obj, p_method, p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::BUG, "");
}


#define VECT2_GET(m_t)                                            \
	if (p_name == "x" || p_name == "width") {                     \
		return (*DATA_PTR_CONST(STRCAT2(Vect2, m_t))).x;          \
	} else if (p_name == "y" || p_name == "height") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect2, m_t))).y;          \
	} else THROW_ERROR(VarError::INVALID_GET_NAME, "")

#define VECT3_GET(m_t)                                            \
	if (p_name == "x" || p_name == "width") {                     \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).x;          \
	} else if (p_name == "y" || p_name == "height") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).y;          \
	} else if (p_name == "z" || p_name == "depth") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).y;          \
	} else THROW_ERROR(VarError::INVALID_GET_NAME, "")

var var::get_member(const String& p_name) {
	switch (type) {
		case var::_NULL:   THROW_ERROR(VarError::NULL_POINTER, "");
		case var::BOOL:    THROW_ERROR(VarError::INVALID_GET_NAME, String::format("boolean has no attribute \"%s\"", p_name.c_str()));
		case var::INT:     THROW_ERROR(VarError::INVALID_GET_NAME, String::format("int has no attribute \"%s\"", p_name.c_str()));
		case var::FLOAT:   THROW_ERROR(VarError::INVALID_GET_NAME, String::format("float has no attribute \"%s\"", p_name.c_str()));
		case var::VECT2F:  VECT2_GET(f);
		case var::VECT2I:  VECT2_GET(i);
		case var::VECT3F:  VECT3_GET(f);
		case var::VECT3I:  VECT3_GET(i);
		case var::STRING:
		case var::ARRAY:
		case var::MAP:
			THROW_ERROR(VarError::INVALID_GET_NAME, String::format("member %s does not exists on %s", p_name.c_str(), get_type_name().c_str()));
		case var::OBJECT: return Object::get_member(_data._obj, p_name);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::BUG, "");
}
#undef VECT2_GET
#undef VECT3_GET

#define VECT2_SET(m_t, m_cast)                                                                                      \
	if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) THROW_ERROR(VarError::INVALID_SET_VALUE, ""); \
	if (p_name == "x" || p_name == "width") {                                                                       \
		(*DATA_PTR(STRCAT2(Vect2, m_t))).x = p_value.operator m_cast();                                             \
	} else if (p_name == "y" || p_name == "height") {			                                                    \
		(*DATA_PTR(STRCAT2(Vect2, m_t))).y = p_value.operator m_cast();                                             \
	} else THROW_ERROR(VarError::INVALID_GET_NAME, "")

#define VECT3_SET(m_t, m_cast)                                                                                      \
	if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) THROW_ERROR(VarError::INVALID_SET_VALUE, ""); \
	if (p_name == "x" || p_name == "width") {                                                                       \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).x = p_value.operator m_cast();                                             \
	} else if (p_name == "y" || p_name == "height") {			                                                    \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).y = p_value.operator m_cast();                                             \
	} else if (p_name == "z" || p_name == "depth") {			                                                    \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).y = p_value.operator m_cast();                                             \
	} else THROW_ERROR(VarError::INVALID_GET_NAME, "")

void var::set_member(const String& p_name, var& p_value) {
	switch (type) {
		case var::_NULL:  THROW_ERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_ERROR(VarError::INVALID_GET_NAME, String::format("boolean has no attribute \"%s\"", p_name.c_str()));
		case var::INT:    THROW_ERROR(VarError::INVALID_GET_NAME, String::format("int has no attribute \"%s\"", p_name.c_str()));
		case var::FLOAT:  THROW_ERROR(VarError::INVALID_GET_NAME, String::format("float has no attribute \"%s\"", p_name.c_str()));
		case var::VECT2F: VECT2_SET(f, double); return;
		case var::VECT2I: VECT2_SET(i, int64_t); return;
		case var::VECT3F: VECT3_SET(f, double); return;
		case var::VECT3I: VECT3_SET(i, int64_t); return;
		case var::STRING:
		case var::ARRAY:
		case var::MAP:
			THROW_ERROR(VarError::INVALID_GET_NAME, String::format("member \"%s\" does not exists on \"%s\"", p_name.c_str(), get_type_name().c_str()));
		case var::OBJECT: Object::set_member(_data._obj, p_name, p_value);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::BUG, "");
}
#undef VECT2_SET
#undef VECT3_SET

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL:  return false;
		case BOOL:   return _data._bool;
		case INT:    return _data._int != 0;
		case FLOAT:  return _data._float != 0;
		case STRING: return _data._string.size() != 0;

		case VECT2F: return *DATA_PTR_CONST(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR_CONST(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case ARRAY:  return !_data._arr.empty();
		case MAP:    return !_data._map.empty();
		case OBJECT: return _data._obj.operator bool();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"bool\"", get_type_name().c_str()));
}

var::operator int64_t() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		//case STRING: return  _data._string.to_int(); // throws std::exception
	}
	THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"int\"", get_type_name().c_str()));
}

var::operator double() const {
	switch (type) {
		case BOOL: return (double)_data._bool;
		case INT: return (double)_data._int;
		case FLOAT: return _data._float;
		//case STRING: return  _data._string.to_float();
	}
	THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"float\"", get_type_name().c_str()));
}

var::operator String() const {
	if (type != STRING)
		THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"String\"", get_type_name().c_str()));
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::BUG, "");
}

#define VAR_VECT_CAST(m_dim, m_t)                                                       \
var::operator D_VEC(Vect, m_dim, m_t)() const {                                         \
	switch (type) {                                                                     \
		case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, f));      \
		case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, i));      \
		default: THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"" STRINGIFY(D_VEC(VECT, m_dim, I)) "\"", get_type_name().c_str()));\
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
	THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"Array\"", get_type_name().c_str()));
}

var::operator Map() const {
	if (type == MAP) {
		return _data._map;
	}
	THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"Map\"", get_type_name().c_str()));
}

var::operator ptr<Object>() const {
	if (type == OBJECT) {
		return _data._obj;
	}
	THROW_ERROR(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"Object\"", get_type_name().c_str()));
}

/* operator overloading */
		/* comparison */
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                           \
switch (p_other.type) {                                                                                                             \
	case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f));  \
	case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i));  \
}                                                                                                                                   \
break;

#define THROW_OPERATOR_NOT_SUPPORTED(m_op)                                                         \
do {                                                                                               \
	THROW_ERROR(VarError::OPERATOR_NOT_SUPPORTED,                                                  \
		String::format("operator \"" STR(m_op) "\" not supported on operands \"%s\" and \"%s\"",   \
			get_type_name().c_str(), p_other.get_type_name().c_str())                              \
	);                                                                                             \
} while(false)

bool var::operator==(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:   {
			switch (p_other.type) {
				case BOOL:  return _data._bool == p_other._data._bool;
				case INT:   return _data._bool == (bool) p_other._data._int;
				case FLOAT: return _data._bool == (bool) p_other._data._float;
			}
		} break;
		case INT:    {
			switch (p_other.type) {
				case BOOL:  return (bool) _data._int == p_other._data._bool;
				case INT:   return _data._int        == p_other._data._int;
				case FLOAT: return _data._int        == p_other._data._float;
			}
		} break;
		case FLOAT:  {
			switch (p_other.type) {
				case BOOL:  return (bool)_data._float == p_other._data._bool;
				case INT:   return _data._float       == p_other._data._int;
				case FLOAT: return _data._float       == p_other._data._float;
			}
		} break;
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
			return _data._obj->__eq(p_other);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_ERROR(VarError::BUG, "");
}

bool var::operator!=(const var& p_other) const {
	return !operator==(p_other);
}

bool var::operator<(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int)    _data._bool < (int)p_other._data._bool;
				case INT:   return (int)    _data._bool < p_other._data._int;
				case FLOAT: return (double) _data._bool < p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int < (int)p_other._data._bool;
				case INT:   return _data._int < p_other._data._int;
				case FLOAT: return _data._int < p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float < (double) p_other._data._bool;
				case INT:   return _data._float < p_other._data._int;
				case FLOAT: return _data._float < p_other._data._float;
			}
		} break;
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
			return _data._obj->__lt(p_other);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	// FIXME: a workaround for map keys as vars.
	return this < &p_other;
}

bool var::operator>(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int)    _data._bool > (int) p_other._data._bool;
				case INT:   return (int)    _data._bool > p_other._data._int;
				case FLOAT: return (double) _data._bool > p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int > (int) p_other._data._bool;
				case INT:   return _data._int > p_other._data._int;
				case FLOAT: return _data._int > p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float > (int)p_other._data._bool;
				case INT:   return _data._float > p_other._data._int;
				case FLOAT: return _data._float > p_other._data._float;
			}
		} break;
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
			return _data._obj->__gt(p_other);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
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
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t) _data._bool + (int64_t) p_other._data._bool;
				case INT:   return (int64_t) _data._bool + p_other._data._int;
				case FLOAT: return (double) _data._bool  + p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int          + (int64_t) p_other._data._bool;
				case INT:   return _data._int          + p_other._data._int;
				case FLOAT: return (double) _data._int + p_other._data._float;
			}

		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float + (double) p_other._data._bool;
				case INT:   return _data._float + (double) p_other._data._int;
				case FLOAT: return _data._float + p_other._data._float;
			}
		} break;
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
			break;
		case OBJECT:
			return _data._obj->__add(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(+);
}

var var::operator-(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t) _data._bool - (int64_t) p_other._data._bool;
				case INT:   return (int64_t) _data._bool - p_other._data._int;
				case FLOAT: return (double) _data._bool  - p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int         - (int64_t)p_other._data._bool;
				case INT:   return _data._int         - p_other._data._int;
				case FLOAT: return (double)_data._int - p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float - (double)p_other._data._bool;
				case INT:   return _data._float - (double)p_other._data._int;
				case FLOAT: return _data._float - p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, -) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, -) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, -) } break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__sub(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(-);
}

var var::operator *(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t)_data._bool * (int64_t)p_other._data._bool;
				case INT:   return (int64_t)_data._bool * p_other._data._int;
				case FLOAT: return (double)_data._bool  * p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int         * (int64_t)p_other._data._bool;
				case INT:   return _data._int         * p_other._data._int;
				case FLOAT: return (double)_data._int * p_other._data._float;
				case STRING: {
					String ret;
					for (int64_t i = 0; i < _data._int; i++) {
						ret += p_other._data._string;
					}
					return ret;
				}
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float * (double)p_other._data._bool;
				case INT:   return _data._float * (double)p_other._data._int;
				case FLOAT: return _data._float * p_other._data._float;
			}
		} break;
		case STRING:
			if (p_other.type == INT) {
				String ret;
				for (int64_t i = 0; i < p_other._data._int; i++) {
					ret += _data._string;
				}
				return ret;
			}
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, *) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, *) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, *) } break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(*);
}

var var::operator /(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return (double)_data._bool  / p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return _data._int         / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return _data._int         / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return (double)_data._int / p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					return _data._float / p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, /) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, /) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, /) } break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__div(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(/);
}

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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(%);
}
#undef VAR_RET_EQUAL
#undef VAR_SWITCH_VECT

#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                               \
switch (p_other.type) {                                                                                                                 \
	case D_VEC(VECT, m_dim, F): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f)); return *this;     \
	case D_VEC(VECT, m_dim, I): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i)); return *this;     \
}                                                                                                                                       \
break;

var& var::operator+=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    + (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    + p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool + p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   =         _data._int + (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int + p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int + p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float + (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float + (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float + p_other._data._float;         return *this;
			}
		} break;
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
			break;
		case OBJECT:
			return _data._obj->__add_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(+=);
}

var& var::operator-=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    - (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    - p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool - p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int)   _data._int - (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int - p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int - p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float - (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float - (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float - p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, -=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, -=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, -=) } break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__sub_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(-=);
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    * (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    * p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool * p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int)   _data._int * (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int * p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int * p_other._data._float;      return *this;
				case STRING: {
					String self;
					for (int64_t i = 0; i < _data._int; i++) {
						self += p_other._data._string;
					}
					type = STRING; new(&_data._string) String(self); return *this;
				}
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float * (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float * (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float * p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			if (p_other.type == INT) {
				String self;
				for (int64_t i = 0; i < p_other._data._int; i++) {
					self += _data._string;
				}
				_data._string = self; return *this;
			}
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, *=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, *=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, *=) } break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	THROW_OPERATOR_NOT_SUPPORTED(*=);
}

var& var::operator/=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_ERROR(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / (int)p_other._data._bool; return *this;
				case INT:
					if (p_other._data._int == 0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / p_other._data._int;       return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._bool / p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_ERROR(VarError::ZERO_DIVISION, "");
					_data._int   = (int)   _data._int / (int)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					_data._int   =         _data._int / p_other._data._int;        return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._int / p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_ERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._int;   return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_ERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, /=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, /=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, /=) } break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
}


String var::get_type_name_s(var::Type p_type) {
	static const char* _type_names[_TYPE_MAX_] = {
		"null",
		"var",
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 13);
	return _type_names[p_type];
}

String var::get_type_name() const {
	if (type == OBJECT) {
		return _data._obj->get_class_name();
	} else {
		return get_type_name_s(type);
	}
}

}

#undef VAR_CASE_OP
#undef VAR_SWITCH_VECT

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
