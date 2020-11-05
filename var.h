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

#ifdef ___________________USAGE___________________
// USAGE:
//     Note that you must define VAR_IMPLEMENTATION in exactly one source file. Use
//     UNDEF_VAR_DEFINES to undef var.h macros (like DEBUG_BREAK, DEBUG_PRINT...)
//     if you don't want them.

#define VAR_IMPLEMENTATION
#include "var.h"
using namespace varh;

class Aclass : public Object {
	REGISTER_CLASS(Aclass, Object) {
		BIND_ENUM_VALUE("EVAL1", EVAL1);                          // Bind unnamed enums
		BIND_ENUM_VALUE("EVAL2", EVAL2);

		BIND_ENUM("MyEnum", {                                     // enum type
			{ "V1", MyEnum::V1 },
			{ "V2", MyEnum::V2 },
			{ "V3", MyEnum::V3 },
		});

		BIND_METHOD("method1",  &Aclass::method1);                // bind method with 0 arguments
		BIND_METHOD("method2",  &Aclass::method2,  PARAMS("arg1", "arg2"), DEFVALUES("arg2_defval")); // method with 2 arguments, 1 default arg.
		BIND_STATIC_FUNC("static_func", &Aclass::static_fun);     // similerly for static functions.

		BIND_MEMBER("member", &Aclass::member);                   // bind member, type MUST BE var.
		BIND_MEMBER("member", &Aclass::member, DEFVAL(2));        // bind member with default initial value.

		BIND_STATIC_MEMBER("s_member", &Aclass::s_member);        // no need to set default value, could be resolved statically. type MUST BE var.
		BIND_STATIC_MEMBER("s_member", &Aclass::s_member);        // no need to set default value, could be resolved statically.

		BIND_CONST("CONST_VAL", &Aclass::CONST_VAL);              // similerly as static member but it could be any type since it won't change.

		BIND_METHOD_VA("va_method", &Aclass::va_method);          // method must be `Ret va_method(stdvec<var>& p_args) {}`
		BIND_STATIC_FUNC_VA("va_func_s", &Aclass::va_func_s);     // similerly static method.
	}
};

#include <iostream>
#define print(x) std::cout << (x) << std::endl
int main() {
	var v;
	v = 3.14;           print(v); // prints 3.14 float
	v = "hello world!"; print(v); // prints the String

	v = String("string"); v = Vect2f(1.2, 3.4); v = Vect2i(1, 2);
	v = Map(); v = Array(1, 2.3, "hello world!", Array(4, 5, 6));

	class Aclass : public Object {
	public: String to_string() const { return "Aclass"; }
	};
	v = newptr<Aclass>(); print(v); // prints Aclass
}

#endif // ___________________USAGE___________________

#define VAR_H_HEADER_ONLY

#ifndef _VAR_H
#define _VAR_H


#ifndef STRING_H
#define STRING_H


#ifndef VARHCORE_H
#define VARHCORE_H

#include <assert.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <type_traits>
#include <typeinfo>

#define _USE_MATH_DEFINES
#include <map>
#include <unordered_map>
#include <math.h>
#include <vector>

// Reference : https://stackoverflow.com/questions/2111667/compile-time-string-hashing


// CRC32 Table (zlib polynomial)
// Reference : https://stackoverflow.com/questions/2111667/compile-time-string-hashing

static constexpr uint32_t crc_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

template<size_t idx>
constexpr uint32_t crc32(const char* str)
{
	return (crc32<idx - 1>(str) >> 8) ^ crc_table[(crc32<idx - 1>(str) ^ str[idx]) & 0x000000FF];
}

// This is the stop-recursion function
template<>
constexpr uint32_t crc32<size_t(-1)>(const char* str)
{
	return 0xFFFFFFFF;
}

// This doesn't take into account the nul char
#define COMPILE_TIME_CRC32_STR(x) (crc32<sizeof(x) - 2>(x) ^ 0xFFFFFFFF)

unsigned constexpr __const_hash(char const* input) {
	return *input ? static_cast<unsigned int>(*input) + 33 * __const_hash(input + 1) : 5381;
}

std::size_t constexpr operator "" _hash(const char* s, size_t) {
	return __const_hash(s);
}
// -----------------------------------------------------------------------------------

#define func var

#define STR(m_mac) #m_mac
#define STRINGIFY(m_mac) STR(m_mac)
#define PLACE_HOLDER_MACRO


#define MISSED_ENUM_CHECK(m_max_enum, m_max_value) \
	static_assert((int)m_max_enum == m_max_value, "MissedEnum: " STRINGIFY(m_max_enum) " != " STRINGIFY(m_value) \
		"\n\tat: " __FILE__ "(" STRINGIFY(__LINE__) ")")


namespace varh {
template<typename T>
using ptr = std::shared_ptr<T>;

template<typename T>
using stdvec = std::vector<T>;

// TODO: refactor map to has table (unordered_map) it require hash<var>() implementation.
template<typename T1, typename T2>
using stdmap = std::map<T1, T2>;

template<typename T1, typename T2>
using stdhashtable = std::unordered_map<T1, T2>;

template<typename T, typename... Targs>
inline ptr<T> newptr(Targs... p_args) {
	return std::make_shared<T>(p_args...);
}

template<typename T1, typename T2>
inline ptr<T1> ptrcast(T2 p_ptr) {
	return std::static_pointer_cast<T1>(p_ptr);
}

template<typename T, typename... Targs>
stdvec<T> make_stdvec(Targs... p_args) { return { p_args... }; }

#define VSNPRINTF_BUFF_SIZE 8192

#if defined(DOUBLE_AS_REAL)
typedef double real_t;
#else
typedef float real_t;
#endif
} // namespace varh

#ifndef DEBUG_BUILD
#if defined(_DEBUG) || defined(DEBUG)
#define DEBUG_BUILD
#endif
#endif

#ifdef DEBUG_BUILD
#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif
#else
#define DEBUG_BREAK()
#endif

#define DEBUG_PRINT(m_msg, ...) \
	printf("DEBUG: %s\n\tat: %s(%s:%li)", m_msg, __FUNCTION__, __FILE__, __LINE__)

#if defined(DEBUG_BUILD)
#define VAR_ASSERT(m_cond)                                                                                            \
	if (!(m_cond)) {                                                                                                  \
		printf("ASSERTION FAILED: %s = false\n\tat: %s(%s:%li)", #m_cond, __FUNCTION__, __FILE__, __LINE__);          \
		DEBUG_BREAK();                                                                                                \
	} else (void(0))
#else
#define VAR_ASSERT
#endif

#define THROW_VARERROR(m_type, m_msg) throw VarError(m_type, m_msg)_ERR_ADD_DBG_VARS
#ifdef DEBUG_BUILD
	#define _ERR_ADD_DBG_VARS .set_deg_variables(__FUNCTION__, __FILE__, __LINE__)
#else
	#define _ERR_ADD_DBG_VARS 
#endif

namespace varh {

class String;
class var;

class MemberInfo {
public:
	enum Type {
		METHOD,
		PROPERTY,
		ENUM,
		ENUM_VALUE,
	};

	virtual Type get_type() const = 0;
	virtual const String& get_name() const = 0;

protected:
	Type type;
};

class _Iterator {
public:
	virtual bool __iter_has_next() = 0;
	virtual var __iter_next() = 0;
};

class VarError : public std::exception {
public:
	enum Type {
		OK = 0,
		BUG,

		NULL_POINTER,
		OPERATOR_NOT_SUPPORTED,
		NOT_IMPLEMENTED,
		ZERO_DIVISION,
		TYPE_ERROR,
		ATTRIBUTE_ERROR,
		INVALID_ARG_COUNT,
		INVALID_INDEX,

		_ERROR_MAX_,
	};

	const char* what() const noexcept override { return msg.c_str(); }
	Type get_type() const { return type; }
	static std::string get_err_name(Type p_type);

	VarError() {}
	VarError(Type p_type) { type = p_type; }
	VarError(Type p_type, const std::string& p_msg) {
		type = p_type;
		msg = p_msg;
	}
	
#ifdef DEBUG_BUILD
	const std::string& get_dbg_func() const { return __dbg_func__; }
	const std::string& get_dbg_file() const { return __dbg_file__; }
	int get_dbg_line() const { return __dbg_line__; }

	VarError& set_deg_variables(const std::string& p_func, const std::string& p_file, uint32_t p_line) {
		__dbg_func__ = p_func;
		__dbg_file__ = p_file;
		__dbg_line__ = p_line;
		return *this;
	}
#endif

private:
	Type type = OK;
	std::string msg;

#ifdef DEBUG_BUILD
	std::string __dbg_func__ = "<NO-FUNC-SET>";
	std::string __dbg_file__ = "<NO-FILE-SET>";
	uint32_t __dbg_line__ = 0;
#endif

};
}

#endif // VARHCORE_H

namespace varh {

class var;
class Object;

class String {
public:

	// Constructors.
	String() : _data(new std::string(""))       {}
	String(const std::string& p_copy)  { _data = new std::string(p_copy); }
	String(const char* p_copy)         { _data = new std::string(p_copy); }
	String(const String& p_copy)       {
		if (p_copy._data) _data = new std::string(*p_copy._data);
	}
	String(char p_char)                         { _data = new std::string(1, p_char); }
	explicit String(int p_i)                    { _data = new std::string(std::to_string(p_i).c_str()); }
	explicit String(int64_t p_i)                { _data = new std::string(std::to_string(p_i).c_str()); }
	explicit String(size_t p_i)                 { _data = new std::string(std::to_string(p_i).c_str()); }
	explicit String(double p_d)                 { _data = new std::string(std::to_string(p_d).c_str()); }
	~String() { delete _data; }
	constexpr static  const char* get_class_name_s() { return "String"; }

	// reflection methods.
	var call_method(const String& p_method, const stdvec<var*>& p_args);
	static const stdmap<String, const MemberInfo*>& get_member_info_list();
	static bool has_member(const String& p_member);
	static const MemberInfo* get_member_info(const String& p_member);

	static String format(const char* p_format, ...);
	int64_t to_int() const {
		// TODO: this should throw std::exceptions
		if (startswith("0x") || startswith("0X")) {
			return std::stoll(*_data, nullptr, 16);
		} else if (startswith("0b") || startswith("0B")) {
			return std::stoll(substr(2, size()), nullptr, 2);
		} else {
			return std::stoll(*_data);
		}
	} 
	double to_float() const { return std::stod(*_data); }
	String get_line(uint64_t p_line) const;
	size_t hash() const { return std::hash<std::string>{}(*_data); }
	size_t const_hash() const { return __const_hash(_data->c_str()); }

	String substr(size_t p_start, size_t p_end) const;
	bool endswith(const String& p_str) const;
	bool startswith(const String& p_str) const;
	// String strip(p_delemeter = " "); lstrip(); rstrip();
	// Array split(p_delimeter = " ");

	// operators.
	char operator[](int64_t p_index) const {
		if (0 <= p_index && p_index < (int64_t)size())
			return (*_data)[p_index];
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return (*_data)[size() + p_index];
		throw VarError(VarError::INVALID_INDEX, String::format("String index %i is invalid.", p_index));
	}
	char& operator[](int64_t p_index) {
		if (0 <= p_index && p_index < (int64_t)size())
			return (*_data)[p_index];
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return (*_data)[size() + p_index];
		throw VarError(VarError::INVALID_INDEX, String::format("String index %i is invalid.", p_index));
	}

	operator std::string() const                   { return *_data; }
	// operator bool() {} don't implement this don't even delete
	
	bool operator==(const String & p_other) const  { return *_data == *p_other._data; }
	bool operator!=(const String & p_other) const  { return *_data != *p_other._data; }
	bool operator<(const String& p_other) const    { return *_data < *p_other._data; }

	String operator+(char p_c) const               { return *_data + p_c; }
	String operator+(int p_i) const                { return *_data + std::to_string(p_i); }
	String operator+(double p_d) const             { return *_data + std::to_string(p_d); }
	String operator+(const char* p_cstr) const     { return *_data + p_cstr; }
	String operator+(const String& p_other) const  { return *_data + *p_other._data; }
	// String operator+(var& p_obj) const          { TODO: }

	String& operator+=(char p_c)                   { *_data += p_c;                 return *this; }
	String& operator+=(int p_i)                    { *_data += std::to_string(p_i); return *this; }
	String& operator+=(double p_d)                 { *_data += std::to_string(p_d); return *this; }
	String& operator+=(const char* p_cstr)         { *_data += p_cstr;              return *this; }
	String& operator+=(const String& p_other)      { *_data += *p_other._data;      return *this; }
	// String& operator+(var& p_obj)               { TODO: }

	String& operator=(char p_c)                   { *_data = p_c;                 return *this; }
	String& operator=(int p_i)                    { *_data = std::to_string(p_i); return *this; }
	String& operator=(double p_d)                 { *_data = std::to_string(p_d); return *this; }
	String& operator=(const char* p_cstr)         { *_data = p_cstr;              return *this; }
	String& operator=(const String& p_other)      { *_data = *p_other._data;      return *this; }
	// String& operator=(var& p_obj)               { TODO: }

	// Wrappers.
	size_t size() const                   { return _data->size(); }
	const char* c_str() const             { return _data->c_str(); }
	String& append(const String& p_other) { _data->append(p_other); return *this; }

private:
	friend class var;
	friend std::ostream& operator<<(std::ostream& p_ostream, const String& p_str);
	friend std::istream& operator>>(std::istream& p_ostream, String& p_str);
	std::string* _data = nullptr;
};

// Global operators. TODO: implement more
bool operator==(const char* p_cstr, const String& p_str);
bool operator!=(const char* p_cstr, const String& p_str);


}

#endif // STRING_H

#ifndef  ARRAY_H
#define  ARRAY_H

//include "varhcore.h"

namespace varh {

class Array {
public:
	Array() {
		_data = newptr<stdvec<var>>();
	}
	Array(const ptr<stdvec<var>>& p_data) {
		_data = p_data;
	}
	Array(const Array& p_copy) {
		_data = p_copy._data;
	}
	Array(const stdvec<var>& p_data);

	template <typename... Targs>
	Array(Targs... p_args) {
		_data = newptr<stdvec<var>>();
		_make_va_arg_array(p_args...);
	}

	stdvec<var>* get_data() {
		return _data.operator->();
	}
	stdvec<var>* get_data() const {
		return _data.operator->();
	}
	constexpr static  const char* get_class_name_s() { return "Array"; }

	Array copy(bool p_deep = true) const;

	// reflection methods.
	var call_method(const String& p_method, const stdvec<var*>& p_args);
	static const stdmap<String, const MemberInfo*>& get_member_info_list();
	static bool has_member(const String& p_member);
	static const MemberInfo* get_member_info(const String& p_member);

	// Wrappers.
	// TODO: throw all errors with VarError
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void push_back(const var& p_var) { _data->push_back(p_var); }
	void pop_back() { _data->pop_back(); }
	Array& append(const var& p_var) { push_back(p_var); return *this; }
	var pop();
	void clear() { (*_data).clear(); }
	void insert(int64_t p_index, const var& p_var) { _data->insert(_data->begin() + p_index, p_var); }
	var& at(int64_t p_index) {
		if (0 <= p_index && p_index < (int64_t)size())
			return (*_data).at(p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return (*_data).at(size() + p_index);
		throw VarError(VarError::INVALID_INDEX, String::format("Array index %i is invalid.", p_index));
	}
	void resize(size_t p_size) { _data->resize(p_size); }
	void reserve(size_t p_size) { _data->reserve(p_size); }

	std::vector<var>::const_iterator begin() const { return (*_data).begin(); }
	std::vector<var>::const_iterator end() const { return (*_data).end(); }
	var& back() { return (*_data).back(); }
	var& front() { return (*_data).front(); }

	var& operator[](int64_t p_index) const {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data->operator[](p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data->operator[](size() + p_index);
		throw VarError(VarError::INVALID_INDEX, String::format("Array index %i is invalid.", p_index));
	}
	var& operator[](int64_t p_index) { 
		if (0 <= p_index && p_index < (int64_t)size())
			return _data->operator[](p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data->operator[](size() + p_index);
		throw VarError(VarError::INVALID_INDEX, String::format("Array index %i is invalid.", p_index));
	}
	// TODO: 

	// Operators.
	operator bool() const { return empty(); }
	String to_string() const; // operator String() const;
	bool operator ==(const Array& p_other) const;
	Array& operator=(const Array& p_other);
	Array operator+(const Array& p_other) const;
	Array& operator+=(const Array& p_other);

private:
	friend class var;
	ptr<std::vector<var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr);

	// va_args constructor internal
	template <typename T, typename... Targs>
	constexpr void _make_va_arg_array(T p_val, Targs... p_args) {
		push_back(p_val);
		_make_va_arg_array(p_args...);
	}
	void _make_va_arg_array() { return; }
};

}

#endif // ARRAY_H

#ifndef  MAP_H
#define  MAP_H

//include "varhcore.h"


namespace varh {

class var;
class String;

class Map {
public:
	struct _KeyValue;
	typedef stdhashtable<size_t, _KeyValue> _map_internal_t;

	// Mehtods.
	Map();
	Map(const ptr<_map_internal_t>& p_data);
	Map(const Map& p_copy);
	constexpr static  const char* get_class_name_s() { return "Map"; }

	_map_internal_t* get_data() { return  _data.operator->(); }
	_map_internal_t* get_data() const { return _data.operator->(); }

	Map copy(bool p_deep = true) const;

	// reflection methods.
	var call_method(const String& p_method, const stdvec<var*>& p_args);
	static const stdmap<String, const MemberInfo*>& get_member_info_list();
	static bool has_member(const String& p_member);
	static const MemberInfo* get_member_info(const String& p_member);

	// Wrappers.
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void insert(const var& p_key, const var& p_value);
	var operator[](const var& p_key) const;
	var& operator[](const var& p_key);
	var operator[](const char* p_key) const;
	var& operator[](const char* p_key);
	_map_internal_t::iterator begin() const;
	_map_internal_t::iterator end() const;
	_map_internal_t::iterator find(const var& p_key) const;
	void clear();
	bool has(const var& p_key) const;
	// TODO: abstract iteration in var and implement here.
	// TODO: add more

	// Operators.
	operator bool() const { return empty(); }
	String to_string() const; // operator String() const;
	bool operator ==(const Map& p_other) const;
	Map& operator=(const Map& p_other);

private:
	friend class var;
	
	//ptr<stdmap<var, var>> _data;
	ptr<_map_internal_t> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Map& p_dict);
};

}

#endif // MAP_H

#ifndef OBJECT_H
#define OBJECT_H

//include "varhcore.h"

#define REGISTER_CLASS(m_class, m_inherits)                                                          \
public:                                                                                              \
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
	var operator()(stdvec<var>& p_vars);

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
	virtual var __call(stdvec<var*>& p_vars);                                                // instance(args)

	// the dynamic way to call method on native classes. See DynamicLibrary for reference.
	constexpr static const char* __call_method = "__call_method";  

	static var get_member(ptr<Object> p_self, const String& p_name);
	static void set_member(ptr<Object> p_self, const String& p_name, var& p_value);

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

#ifndef VECTOR_H
#define VECTOR_H

//include "varhcore.h"

#define VECT2_DEFAULT_CMP_OP_OVERLOAD(m_op)            \
bool operator m_op (const Vect2<T>& p_other) const {   \
	return get_length() m_op p_other.get_length();     \
}
#define VECT3_DEFAULT_CMP_OP_OVERLOAD(m_op)            \
bool operator m_op (const Vect3<T>& p_other) const {   \
	return get_length() m_op p_other.get_length();     \
}

namespace varh {

template<typename T>
struct Vect2 {
	union { T x, width; };
	union { T y, height; };

	// Constructors
	Vect2(T p_x = 0, T p_y = 0) : x(p_x), y(p_y) {}
	Vect2(const T* p_arr) : x(p_arr[0]), y(p_arr[1]) {}
	template<typename T2>
	Vect2(const Vect2<T2>& p_copy) : x((T)p_copy.x), y((T)p_copy.y) {}
	constexpr static  const char* get_class_name_s() { return "Vect2<T>"; }

	// Methods.
	real_t get_length() const {
		return (real_t)sqrtf((real_t)(x * x + y * y));
	}
	float get_angle() const {
		if (x == 0) return (float)M_PI_2;
		return atanf(y / x);
	}

	// Operators.
	Vect2<T> operator+(const Vect2<T>& p_other) const {
		return Vect2<T>(x + p_other.x, y + p_other.y);
	}
	Vect2<T> operator-(const Vect2<T>& p_other) const {
		return Vect2<T>(x - p_other.x, y - p_other.y);
	}
	Vect2<T> operator*(const Vect2<T>& p_other) const {
		return Vect2<T>(x * p_other.x, y * p_other.y);
	}
	template <typename T2>
	Vect2<T> operator*(T2 p_val) const {
		return Vect2<T>(x * p_val, y * p_val);
	}
	Vect2<T> operator/(const Vect2<T>& p_other) const {
		if (p_other.x == 0 || p_other.y == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		return Vect2<T>(x / p_other.x, y / p_other.y);
	}

	VECT2_DEFAULT_CMP_OP_OVERLOAD(<);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(<=);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>=);
	bool operator==(const Vect2<T>& p_other) const {
		return x == p_other.x && y == p_other.y;
	}
	bool operator!=(const Vect2<T>& p_other) const {
		return !operator==(p_other);
	}

	Vect2<T>& operator+=(const Vect2<T>& p_other) {
		x += p_other.x; y += p_other.y;
		return *this;
	}
	Vect2<T>& operator-=(const Vect2<T>& p_other) {
		x -= p_other.x; y -= p_other.y;
		return *this;
	}

	Vect2<T>& operator*=(const Vect2<T>& p_other) {
		x *= p_other.x; y *= p_other.y;
		return *this;
	}

	Vect2<T>& operator/=(const Vect2<T>& p_other) {
		if (p_other.x == 0 || p_other.y == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		x /= p_other.x; y /= p_other.y;
		return *this;
	}

	operator bool() const { return x == 0 && y == 0; }
	String to_string() const {
		return String("Vect2(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(")");
	}
};


template<typename T>
struct Vect3 {
	union { T x, width; };
	union { T y, height; };
	union { T z, depth; };

	// Constructors.
	Vect3(T p_x = 0, T p_y = 0, T p_z = 0) : x(p_x), y(p_y), z(p_z) {}
	Vect3(const T* p_arr) : x(p_arr[0]), y(p_arr[1]), z(p_arr[2]) {}
	template<typename T2>
	Vect3(const Vect3<T2>& p_copy) : x((T)p_copy.x), y((T)p_copy.y), z((T)p_copy.z) {}
	constexpr static  const char* get_class_name_s() { return "Vect3<T>"; }

	// Methods.
	real_t get_length() const {
		return (real_t)sqrtf((real_t)(x * x + y * y + z * z));
	}

	// Operators.
	Vect3<T> operator+(const Vect3<T>& p_other) const {
		return Vect3<T>(x + p_other.x, y + p_other.y, z + p_other.z);
	}
	Vect3<T> operator-(const Vect3<T>& p_other) const {
		return Vect3<T>(x - p_other.x, y - p_other.y, z - p_other.z);
	}
	Vect3<T> operator*(const Vect3<T>& p_other) const {
		return Vect3<T>(x * p_other.x, y * p_other.y, z * p_other.z);
	}
	Vect3<T> operator/(const Vect3<T>& p_other) const {
		if (p_other.x == 0 || p_other.y == 0 || p_other.z == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		return Vect3<T>(x / p_other.x, y / p_other.y, z / p_other.z);
	}

	VECT2_DEFAULT_CMP_OP_OVERLOAD(<);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(<=);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>=);
	bool operator==(const Vect3<T>& p_other) const {
		return x == p_other.x && y == p_other.y && z == p_other.z;
	}
	bool operator!=(const Vect3<T>& p_other) const {
		return !operator==(p_other);
	}

	Vect3<T>& operator+=(const Vect3<T>& p_other) {
		x += p_other.x; y += p_other.y; z += p_other.z;
		return *this;
	}
	Vect3<T>& operator-=(const Vect3<T>& p_other) {
		x -= p_other.x; y -= p_other.y; z -= p_other.z;
		return *this;
	}
	Vect3<T>& operator*=(const Vect3<T>& p_other) {
		x *= p_other.x; y *= p_other.y; z *= p_other.z;
		return *this;
	}
#define M_OP_TEMPLATE(T2)                    \
	Vect3<T>& operator*=(T2 p_val) const {   \
		x *= p_val; y *= p_val; z *= p_val;  \
		return *this;                        \
	}
	M_OP_TEMPLATE(float)
	M_OP_TEMPLATE(int)
#undef M_OP_TEMPLATE

	template<typename T2>
	Vect3<T>& operator/=(const Vect3<T2>& p_other) {
		if (p_other.x == 0 || p_other.y == 0 || p_other.z == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		x /= (T)p_other.x; y /= (T)p_other.y; z /= (T)p_other.z;
		return *this;
	}

	operator bool() const { return x == 0 && y == 0 && z == 0; }
	String to_string() const {
		return String("Vect3(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(", ")
			.append(std::to_string(z)).append(")");
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect2<T>& p_vect) {
	p_ostream << p_vect.to_string();
	return p_ostream;
}
template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect3<T>& p_vect) {
	p_ostream << p_vect.to_string();
	return p_ostream;
}

/* typedefs */
typedef Vect2<double> Vect2f;
typedef Vect2<int64_t> Vect2i;
typedef Vect3<double> Vect3f;
typedef Vect3<int64_t> Vect3i;

typedef Vect2f Size;
typedef Vect2f Point;

}

#endif //VECTOR_H // not a part of var anymore

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

// TODO: var fn = &func; fn(); operator(){}

namespace varh {

class var {
public:
	enum Type {
		_NULL, // not initialized.
		VAR,   // any type used only for member info.

		BOOL,
		INT,
		FLOAT,
		STRING,

		// misc types
		ARRAY,
		MAP,
		OBJECT,

		_TYPE_MAX_,
	};

	/* constructors */
	var();
	var(const var& p_copy);
	var(bool p_bool);
	var(int p_int);
	var(size_t p_int);
	var(int64_t p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const String& p_string);
	var(const Array& p_array);
	var(const Map& p_map);
	~var();
	
	template <typename T=Object>
	var(const ptr<T>& p_ptr) {
		type = OBJECT;
		new(&_data._obj) ptr<Object>(p_ptr);
	}

	template <typename... Targs>
	var operator()(Targs... p_args) {
		return __call(p_args...);
	}

	// Methods.
	inline Type get_type() const { return type; }
	String get_type_name() const;
	//const char* get_parent_class_name() const;
	size_t hash() const;
	static bool is_hashable(var::Type p_type);
	void clear();
	var copy(bool p_deep = false) const;

	constexpr static const char* get_type_name_s(var::Type p_type) {
		switch (p_type) {
			case var::_NULL:  return "null";
			case var::VAR:    return "var";
			case var::BOOL:   return "bool";
			case var::INT:    return "int";
			case var::FLOAT:  return "float";
			case var::STRING: return "String";
			case var::ARRAY:  return "Array";
			case var::MAP:    return "Map";
			case var::OBJECT: return "Object";
			default:
				return "";
		}
	}

	// Operators.
	operator bool() const;
	operator int64_t() const;
	operator int() const { return (int)operator int64_t(); }
	operator size_t() const { return (size_t)operator int64_t(); }
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator String() const;   // int.operator String() is invalid casting.
	String to_string() const;  // int.to_string() is valid.
	// this treated as: built-in C++ operator[](const char *, int), conflict with operator[](size_t)
	// operator const char* () const;
	operator Array() const;
	operator Map() const;
	operator ptr<Object>() const;

	template <typename T>
	ptr<T> cast_to() const {
		return ptrcast<T>(operator ptr<Object>());
	}

#define _VAR_OP_DECL(m_ret, m_op, m_access)                                                        \
	m_ret operator m_op (bool p_other) m_access { return operator m_op (var(p_other)); }           \
	m_ret operator m_op (int64_t p_other) m_access { return operator m_op (var(p_other)); }        \
	m_ret operator m_op (int p_other)     m_access { return operator m_op (var(p_other)); }        \
	m_ret operator m_op (double p_other) m_access { return operator m_op (var(p_other)); }         \
	m_ret operator m_op (const char* p_other) m_access { return operator m_op (var(p_other)); }    \
	m_ret operator m_op (const var& p_other) m_access
#define VAR_OP_DECL(m_ret, m_op, m_access) _VAR_OP_DECL(m_ret, m_op, m_access)

	VAR_OP_DECL(bool, ==, const);
	VAR_OP_DECL(bool, !=, const);
	VAR_OP_DECL(bool, < , const);
	VAR_OP_DECL(bool, > , const);
	VAR_OP_DECL(bool, <=, const);
	VAR_OP_DECL(bool, >=, const);

	var operator++();
	var operator++(int);
	var operator--();
	var operator--(int);
	bool operator !() const { return !operator bool(); }
	var operator[](const var& p_key) const;
	// var& operator[](const var& p_key); // TODO:

	var __get_mapped(const var& p_key) const;
	void __set_mapped(const var& p_key, const var& p_value);

	var __iter_begin();

	template <typename... Targs>
	var __call(Targs... p_args) {
		stdvec<var> _args = make_stdvec<var>(p_args...);
		stdvec<var*> args; for (var& v : _args) args.push_back(&v);
		return __call_internal(args);
	}
	template <typename... Targs>
	var call_method(const String& p_method, Targs... p_args) {
		stdvec<var> _args = make_stdvec<var>(p_args...);
		stdvec<var*> args; for (var& v : _args) args.push_back(&v);
		return call_method_internal(p_method, args);
	}
	var call_method(const String& p_method, stdvec<var*>& p_args) { return call_method_internal(p_method, p_args); }

	var get_member(const String& p_name);
	void set_member(const String& p_name, var& p_value);

	const MemberInfo* get_member_info(const String& p_name) const;
	static const MemberInfo* get_member_info_s(var::Type p_type, const String& p_name);
	const stdvec<const MemberInfo*> get_member_info_list() const;
	static stdvec<const MemberInfo*> get_member_info_list_s(var::Type p_type);

private:
	var __call_internal(stdvec<var*>& p_args);
	var call_method_internal(const String& p_method, stdvec<var*>& p_args);
public:

	VAR_OP_DECL(var, +, const);
	VAR_OP_DECL(var, -, const);
	VAR_OP_DECL(var, *, const);
	VAR_OP_DECL(var, /, const);
	VAR_OP_DECL(var, %, const);

	/* assignments */
	var& operator=(const var& p_other);
	template<typename T=Object>
	var& operator=(const ptr<T>& p_other) {
		clear_data();
		type = OBJECT;
		_data._obj = ptrcast<Object>(p_other);
		return *this;
	}
	VAR_OP_DECL(var&, +=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, -=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, *=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, /=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, %=, PLACE_HOLDER_MACRO);

private:
	struct VarData {
		VarData() : _float(.0f) {}
		~VarData() {}


		union {
			ptr<Object> _obj;
			Map _map;
			Array _arr;
			String _string;

			bool _bool = false;
			int64_t _int;
			double _float;
		};
	};

	// Methods.
	void copy_data(const var& p_other);
	void clear_data();

	// Members.
	static var tmp;
	Type type = _NULL;
	VarData _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);
};

// ******** MEMBER INFO IMPLEMENTATIONS ******************* //

struct VarTypeInfo {
	var::Type type = var::_NULL;
	const char* class_name = nullptr;
	VarTypeInfo(var::Type p_type = var::VAR) : type(p_type) {}
	VarTypeInfo(var::Type p_type, const char* p_class_name) : type(p_type), class_name(p_class_name) {}

	bool operator==(const VarTypeInfo p_other) const {
		if (type != var::OBJECT) return type == p_other.type;
		return strcmp(class_name, p_other.class_name) == 0;
	}
	bool operator!=(const VarTypeInfo p_other) const { return !(operator==(p_other)); }
};

class MethodInfo : public MemberInfo {
private:
	String name;
	bool _is_static = false;
	int arg_count = 0; // -1 is va args, -2 is unknown
	stdvec<String> arg_names;
	stdvec<var> default_args;
	stdvec<VarTypeInfo> arg_types;
	VarTypeInfo return_type;

public:
	virtual Type get_type() const override { return Type::METHOD; }
	virtual const String& get_name() const override { return name; }

	// complete constructor.
	MethodInfo(
		String p_name,
		stdvec<String> p_arg_names,
		stdvec<VarTypeInfo> p_arg_types = stdvec<VarTypeInfo>(),
		VarTypeInfo p_return_type = var::_NULL,
		bool p__is_static = false,
		stdvec<var> p_default_args = stdvec<var>(),
		int p_arg_count = -2
	) {
		name = p_name;
		arg_names = p_arg_names;
		arg_types = p_arg_types;
		return_type = p_return_type;
		_is_static = p__is_static;
		default_args = p_default_args;

		// if va_arg it should be set manually to -1
		if (p_arg_count == -2) arg_count = (int)arg_names.size();
		else arg_count = p_arg_count;
	}

	// zero parameter constructor
	MethodInfo(
		String p_name,
		VarTypeInfo p_return_type = var::_NULL,
		bool p__is_static = false
	) {
		name = p_name;
		return_type = p_return_type;
		_is_static = p__is_static;
	}

	int get_arg_count() const { return arg_count; }
	int get_default_arg_count() const { return (int)default_args.size(); }
	bool is_static() const { return _is_static; }
	const stdvec<String>& get_arg_names() const { return arg_names; }
	const stdvec<var>& get_default_args() const { return default_args; }
	const stdvec<VarTypeInfo>& get_arg_types() const { return arg_types; }
	VarTypeInfo get_return_type() const { return return_type; }
};

class PropertyInfo : public MemberInfo {
private:
	String name;
	VarTypeInfo datatype;
	var value;
	bool _is_const = false;
	bool _is_static = false;

public:
	virtual Type get_type() const override { return Type::PROPERTY; }
	virtual const String& get_name() const override { return name; }

	PropertyInfo(
		const String& p_name,
		VarTypeInfo p_datatype = var::VAR,
		var p_value = var(),
		bool p__is_const = false,
		bool p__is_static = false
	) {
		name = p_name;
		datatype = p_datatype;
		value = p_value;
		_is_const = p__is_const;
		_is_static = p__is_static;
	}

	bool is_static() const { return _is_static; }
	bool is_const() const { return _is_const; }
	VarTypeInfo get_datatype() const { return datatype; }
	const var& get_value() const { return value; }         // value for constants.
	const var& get_default_value() const { return value; } // defalut_value for variables.
};

class EnumInfo : public MemberInfo {
private:
	String name;
	stdvec<std::pair<String, int64_t>> values;

public:
	virtual Type get_type() const override { return Type::ENUM; }
	virtual const String& get_name() const override { return name; }

	EnumInfo(String p_name, const stdvec<std::pair<String, int64_t>>& p_values) {
		name = p_name;
		values = p_values;
	}

	const stdvec<std::pair<String, int64_t>>& get_values() const { return values; }
};

class EnumValueInfo : public MemberInfo {
private:
	String name;
	int64_t value;

public:
	virtual Type get_type() const override { return Type::ENUM_VALUE; }
	virtual const String& get_name() const override { return name; }

	EnumValueInfo(const String& p_name, int64_t p_value) {
		name = p_name;
		value = p_value;
	}

	int64_t get_value() const { return value; }
};


/////////////////////////////////////////////////////////////////

struct Map::_KeyValue {
	var key;
	var value;
	_KeyValue() {}
	_KeyValue(const var& p_key, const var& p_value) : key(p_key), value(p_value) {}
};

}


// include _native after including everything else.

#ifndef NATIVE_CLASSES_H
#define NATIVE_CLASSES_H

// DO NOT INCLUDE THIS AS IT WON'T WORK SINCE IT HAS DEPENDANCY IN _var.h
// INCLUDE _var.h INSETEAD. IT'S DESIGNED LIKE THIS TO GENERATE SINGLE HEADER.


// !!! AUTO GENERATED DO NOT EDIT !!!

#ifndef NATIVE_BIND_GEN_H
#define NATIVE_BIND_GEN_H

namespace varh {

template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

#define DECLARE_VAR_TYPE(m_var_type, m_T)                                                                                     \
	VarTypeInfo m_var_type;																						              \
	if constexpr (std::is_same<m_T, void>::value) {																              \
		m_var_type = var::_NULL;																				              \
	} else if constexpr (std::is_same<std::remove_const<std::remove_reference<m_T>::type>::type, bool>::value) {              \
		m_var_type = var::BOOL;																					              \
	} else if constexpr (std::numeric_limits<m_T>::is_integer) {												              \
		m_var_type = var::INT;																					              \
	} else if constexpr (std::is_floating_point<m_T>::value) {													              \
		m_var_type = var::FLOAT;																				              \
	} else if constexpr (std::is_same<std::remove_const<std::remove_reference<m_T>::type>::type, String>::value ||            \
			std::is_same<std::remove_const<std::remove_reference<m_T>::type>::type, const char*>::value) {			          \
		m_var_type = var::STRING;																				              \
	} else if constexpr (std::is_same<std::remove_const<std::remove_reference<m_T>::type>::type, Array>::value) {             \
		m_var_type = var::ARRAY;																				              \
	} else if constexpr (std::is_same<std::remove_const<std::remove_reference<m_T>::type>::type, Map>::value) {               \
		m_var_type = var::MAP;																					              \
	} else if constexpr (std::is_same<std::remove_const<std::remove_reference<m_T>::type>::type, var>::value) {               \
		m_var_type = var::VAR;																					              \
	} else if constexpr (is_shared_ptr<m_T>::value) {																	      \
		m_var_type = { var::OBJECT, m_T::element_type::get_class_name_s() };                                                  \
	}


class BindData {
protected:
	const char* name;
	const char* class_name;

public:
	enum Type {
		METHOD,
		STATIC_FUNC,
		MEMBER_VAR,
		STATIC_VAR,
		STATIC_CONST,
		ENUM,
		ENUM_VALUE,
	};
	virtual Type get_type() const = 0;
	virtual const char* get_name() const { return name; }
	virtual const char* get_class_name() const { return class_name; }
	virtual int get_argc() const { DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "invalid call"); }
	virtual const MemberInfo* get_member_info() const = 0;
};

class MethodBind : public BindData {
protected:
	int argc = 0;
	ptr<MethodInfo> mi;

public:
	virtual BindData::Type get_type() const { return BindData::METHOD; }
	virtual int get_argc() const { return argc; }

	virtual var call(ptr<Object> self, stdvec<var*>& args) const = 0;
	const MethodInfo* get_method_info() const { return mi.get(); }
	const MemberInfo* get_member_info() const override { return mi.get(); }
};

class StaticFuncBind : public BindData {
protected:
	int argc;
	ptr<MethodInfo> mi;

public:
	virtual BindData::Type get_type()   const { return BindData::STATIC_FUNC; }
	virtual int get_argc()              const { return argc; }

	virtual var call(stdvec<var*>& args) const = 0;
	const MethodInfo* get_method_info() const { return mi.get(); }
	const MemberInfo* get_member_info() const override { return mi.get(); }
};

// ---------------- MEMBER BIND START --------------------------------------
class PropertyBind : public BindData {
protected:
	ptr<PropertyInfo> pi;
public:
	virtual BindData::Type get_type() const { return BindData::MEMBER_VAR; }
	virtual var& get(ptr<Object> self) const = 0;

	const PropertyInfo* get_prop_info() const { return pi.get(); }
	const MemberInfo* get_member_info() const override { return pi.get(); }
};

template<typename Class>
class _PropertyBind : public PropertyBind {
protected:
	typedef var Class::* member_ptr_t;
	member_ptr_t member_ptr;
public:
	_PropertyBind(const char* p_name, const char* p_class_name, member_ptr_t p_member_ptr, ptr<PropertyInfo> p_pi) {
		name = p_name;
		class_name = p_class_name;
		member_ptr = p_member_ptr;
		pi = p_pi;
	}

	virtual var& get(ptr<Object> self) const override {
		return ptrcast<Class>(self).get()->*member_ptr;
	}
};

template<typename Class>
ptr<PropertyBind> _bind_member(const char* p_name, const char* p_class_name, var Class::* p_member_ptr, const var& p_value = var()) {
	var Class::* member_ptr = p_member_ptr;
	return newptr<_PropertyBind<Class>>(p_name, p_class_name, member_ptr, newptr<PropertyInfo>(p_name, var::VAR, p_value));
}
// ------------------------------------------------------------------------


// ---------------- STATIC MEMBER BIND START ------------------------------
class StaticPropertyBind : public BindData {
	var* member = nullptr;
	ptr<PropertyInfo> pi;
public:
	virtual BindData::Type get_type() const { return BindData::STATIC_VAR; }

	StaticPropertyBind(const char* p_name, const char* p_class_name, var* p_member, ptr<PropertyInfo> p_pi) {
		name = p_name;
		class_name = p_class_name;
		member = p_member;
		pi = p_pi;
	}
	virtual var& get() const { return *member; }
	const PropertyInfo* get_prop_info() const { return pi.get(); }
	const MemberInfo* get_member_info() const override { return pi.get(); }
};

inline ptr<StaticPropertyBind> _bind_static_member(const char* p_name, const char* p_class_name, var* p_member) {
	return newptr<StaticPropertyBind>(p_name, p_class_name, p_member, newptr<PropertyInfo>(p_name, var::VAR, *p_member));
}
// ------------------------------------------------------------------------

// ---------------- STATIC CONST BIND START ------------------------------
class ConstantBind : public BindData {
protected:
	ptr<PropertyInfo> pi;
public:
	virtual BindData::Type get_type() const { return BindData::STATIC_CONST; }
	virtual var get() const = 0;

	const PropertyInfo* get_prop_info() const { return pi.get(); }
	const MemberInfo* get_member_info() const override { return pi.get(); }
};

template<typename T>
class _ConstantBind : public ConstantBind {
	T* _const = nullptr;
public:
	_ConstantBind(const char* p_name, const char* p_class_name, T* p_const, ptr<PropertyInfo> p_pi) {
		name = p_name;
		class_name = p_class_name;
		_const = p_const;
		pi = p_pi;
	}

	virtual var get() const override {
		return *_const;
	}
};

template<typename T>
ptr<ConstantBind> _bind_static_const(const char* p_name, const char* p_class_name, T* p_const) {
	DECLARE_VAR_TYPE(datatype, T);
	return newptr<_ConstantBind<T>>(p_name, p_class_name, p_const, newptr<PropertyInfo>(p_name, datatype, *p_const));
}
// ------------------------------------------------------------------------

// ---------------- ENUM BIND START ------------------------------

class EnumBind : public BindData {
	ptr<EnumInfo> ei;
public:
	EnumBind(const char* p_name, const char* p_class_name, ptr<EnumInfo> p_ei) {
		name = p_name;
		class_name = p_class_name;
		ei = p_ei;
	}
	virtual BindData::Type get_type() const { return BindData::ENUM; }
	int64_t get(const String& p_value_name) const {
		for (int i = 0; i < (int)ei->get_values().size(); i++) {
			if (ei->get_values()[i].first == p_value_name) {
				return ei->get_values()[i].second;
			}
		}
		throw VarError(VarError::ATTRIBUTE_ERROR, String::format("value \"%s\" isn't exists on enum %s.", p_value_name.c_str(), name));
	}

	const EnumInfo* get_enum_info() const { return ei.get(); }
	const MemberInfo* get_member_info() const override { return ei.get(); }
};
inline ptr<EnumBind> _bind_enum(const char* p_name, const char* p_class_name, const stdvec<std::pair<String, int64_t>>& p_values) {
	return newptr<EnumBind>(p_name, p_class_name, newptr<EnumInfo>(p_name, p_values));
}

class EnumValueBind : public BindData {
	ptr<EnumValueInfo> evi;
	int64_t value;
public:
	EnumValueBind(const char* p_name, const char* p_class_name, int64_t p_value, ptr<EnumValueInfo> p_evi) {
		name = p_name;
		class_name = p_class_name;
		value = p_value;
		evi = newptr<EnumValueInfo>(p_name, p_value);
	}
	virtual BindData::Type get_type() const { return BindData::ENUM_VALUE; }
	int64_t get() const { return value; }

	const EnumValueInfo* get_enum_value_info() const { return evi.get(); }
	const MemberInfo* get_member_info() const override { return evi.get(); }
};

// -----------------------------------------------------------------------

template<typename T, typename R>
using M0 = R(T::*)();

template<typename T, typename R>
using M0_c = R(T::*)() const;

template<typename T, typename R, typename a0>
using M1 = R(T::*)(a0);

template<typename T, typename R, typename a0>
using M1_c = R(T::*)(a0) const;

template<typename T, typename R, typename a0, typename a1>
using M2 = R(T::*)(a0, a1);

template<typename T, typename R, typename a0, typename a1>
using M2_c = R(T::*)(a0, a1) const;

template<typename T, typename R, typename a0, typename a1, typename a2>
using M3 = R(T::*)(a0, a1, a2);

template<typename T, typename R, typename a0, typename a1, typename a2>
using M3_c = R(T::*)(a0, a1, a2) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
using M4 = R(T::*)(a0, a1, a2, a3);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
using M4_c = R(T::*)(a0, a1, a2, a3) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
using M5 = R(T::*)(a0, a1, a2, a3, a4);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
using M5_c = R(T::*)(a0, a1, a2, a3, a4) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
using M6 = R(T::*)(a0, a1, a2, a3, a4, a5);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
using M6_c = R(T::*)(a0, a1, a2, a3, a4, a5) const;

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
using M7 = R(T::*)(a0, a1, a2, a3, a4, a5, a6);

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
using M7_c = R(T::*)(a0, a1, a2, a3, a4, a5, a6) const;

template<typename R>
using F0 = R(*)();

template<typename R, typename a0>
using F1 = R(*)(a0);

template<typename R, typename a0, typename a1>
using F2 = R(*)(a0, a1);

template<typename R, typename a0, typename a1, typename a2>
using F3 = R(*)(a0, a1, a2);

template<typename R, typename a0, typename a1, typename a2, typename a3>
using F4 = R(*)(a0, a1, a2, a3);

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
using F5 = R(*)(a0, a1, a2, a3, a4);

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
using F6 = R(*)(a0, a1, a2, a3, a4, a5);

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
using F7 = R(*)(a0, a1, a2, a3, a4, a5, a6);

template<typename T, typename R>
class _MethodBind_M0 : public MethodBind {
	M0<T, R> method;
public:
	_MethodBind_M0(const char* p_name, const char* p_class_name, int p_argc, M0<T, R> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 0) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 0 - default_arg_count));
		} else if (args_given > 0) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 0 argument(s).", 0 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 0 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)();
		}
	}
};

template<typename T, typename R>
class _MethodBind_M0_c : public MethodBind {
	M0_c<T, R> method;
public:
	_MethodBind_M0_c(const char* p_name, const char* p_class_name, int p_argc, M0_c<T, R> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 0) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 0 - default_arg_count));
		} else if (args_given > 0) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 0 argument(s).", 0 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 0 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)();
		}
	}
};

template<typename T, typename R, typename a0>
class _MethodBind_M1 : public MethodBind {
	M1<T, R, a0> method;
public:
	_MethodBind_M1(const char* p_name, const char* p_class_name, int p_argc, M1<T, R, a0> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 1) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 1 - default_arg_count));
		} else if (args_given > 1) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 1 argument(s).", 1 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 1 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0]);
		}
	}
};

template<typename T, typename R, typename a0>
class _MethodBind_M1_c : public MethodBind {
	M1_c<T, R, a0> method;
public:
	_MethodBind_M1_c(const char* p_name, const char* p_class_name, int p_argc, M1_c<T, R, a0> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 1) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 1 - default_arg_count));
		} else if (args_given > 1) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 1 argument(s).", 1 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 1 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1>
class _MethodBind_M2 : public MethodBind {
	M2<T, R, a0, a1> method;
public:
	_MethodBind_M2(const char* p_name, const char* p_class_name, int p_argc, M2<T, R, a0, a1> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 2) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 2 - default_arg_count));
		} else if (args_given > 2) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 2 argument(s).", 2 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 2 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1>
class _MethodBind_M2_c : public MethodBind {
	M2_c<T, R, a0, a1> method;
public:
	_MethodBind_M2_c(const char* p_name, const char* p_class_name, int p_argc, M2_c<T, R, a0, a1> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 2) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 2 - default_arg_count));
		} else if (args_given > 2) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 2 argument(s).", 2 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 2 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2>
class _MethodBind_M3 : public MethodBind {
	M3<T, R, a0, a1, a2> method;
public:
	_MethodBind_M3(const char* p_name, const char* p_class_name, int p_argc, M3<T, R, a0, a1, a2> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 3) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 3 - default_arg_count));
		} else if (args_given > 3) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 3 argument(s).", 3 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 3 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2>
class _MethodBind_M3_c : public MethodBind {
	M3_c<T, R, a0, a1, a2> method;
public:
	_MethodBind_M3_c(const char* p_name, const char* p_class_name, int p_argc, M3_c<T, R, a0, a1, a2> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 3) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 3 - default_arg_count));
		} else if (args_given > 3) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 3 argument(s).", 3 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 3 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
class _MethodBind_M4 : public MethodBind {
	M4<T, R, a0, a1, a2, a3> method;
public:
	_MethodBind_M4(const char* p_name, const char* p_class_name, int p_argc, M4<T, R, a0, a1, a2, a3> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 4) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 4 - default_arg_count));
		} else if (args_given > 4) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 4 argument(s).", 4 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 4 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
class _MethodBind_M4_c : public MethodBind {
	M4_c<T, R, a0, a1, a2, a3> method;
public:
	_MethodBind_M4_c(const char* p_name, const char* p_class_name, int p_argc, M4_c<T, R, a0, a1, a2, a3> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 4) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 4 - default_arg_count));
		} else if (args_given > 4) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 4 argument(s).", 4 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 4 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
class _MethodBind_M5 : public MethodBind {
	M5<T, R, a0, a1, a2, a3, a4> method;
public:
	_MethodBind_M5(const char* p_name, const char* p_class_name, int p_argc, M5<T, R, a0, a1, a2, a3, a4> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 5) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 5 - default_arg_count));
		} else if (args_given > 5) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 5 argument(s).", 5 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 5 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
class _MethodBind_M5_c : public MethodBind {
	M5_c<T, R, a0, a1, a2, a3, a4> method;
public:
	_MethodBind_M5_c(const char* p_name, const char* p_class_name, int p_argc, M5_c<T, R, a0, a1, a2, a3, a4> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 5) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 5 - default_arg_count));
		} else if (args_given > 5) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 5 argument(s).", 5 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 5 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
class _MethodBind_M6 : public MethodBind {
	M6<T, R, a0, a1, a2, a3, a4, a5> method;
public:
	_MethodBind_M6(const char* p_name, const char* p_class_name, int p_argc, M6<T, R, a0, a1, a2, a3, a4, a5> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 6) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 6 - default_arg_count));
		} else if (args_given > 6) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 6 argument(s).", 6 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 6 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
class _MethodBind_M6_c : public MethodBind {
	M6_c<T, R, a0, a1, a2, a3, a4, a5> method;
public:
	_MethodBind_M6_c(const char* p_name, const char* p_class_name, int p_argc, M6_c<T, R, a0, a1, a2, a3, a4, a5> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 6) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 6 - default_arg_count));
		} else if (args_given > 6) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 6 argument(s).", 6 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 6 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
class _MethodBind_M7 : public MethodBind {
	M7<T, R, a0, a1, a2, a3, a4, a5, a6> method;
public:
	_MethodBind_M7(const char* p_name, const char* p_class_name, int p_argc, M7<T, R, a0, a1, a2, a3, a4, a5, a6> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 7) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 7 - default_arg_count));
		} else if (args_given > 7) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 7 argument(s).", 7 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 7 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]);
		}
	}
};

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
class _MethodBind_M7_c : public MethodBind {
	M7_c<T, R, a0, a1, a2, a3, a4, a5, a6> method;
public:
	_MethodBind_M7_c(const char* p_name, const char* p_class_name, int p_argc, M7_c<T, R, a0, a1, a2, a3, a4, a5, a6> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 7) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 7 - default_arg_count));
		} else if (args_given > 7) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 7 argument(s).", 7 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 7 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]);
		}
	}
};

template<typename R>
class _StaticFuncBind_F0 : public StaticFuncBind {
	F0<R> static_func;
public:
	_StaticFuncBind_F0(const char* p_name, const char* p_class_name, int p_argc, F0<R> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 0) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 0 - default_arg_count));
		} else if (args_given > 0) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 0 argument(s).", 0 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 0 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(); return var();
		} else {
			return static_func();
		}
	}
};

template<typename R, typename a0>
class _StaticFuncBind_F1 : public StaticFuncBind {
	F1<R, a0> static_func;
public:
	_StaticFuncBind_F1(const char* p_name, const char* p_class_name, int p_argc, F1<R, a0> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 1) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 1 - default_arg_count));
		} else if (args_given > 1) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 1 argument(s).", 1 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 1 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0]); return var();
		} else {
			return static_func(*args[0]);
		}
	}
};

template<typename R, typename a0, typename a1>
class _StaticFuncBind_F2 : public StaticFuncBind {
	F2<R, a0, a1> static_func;
public:
	_StaticFuncBind_F2(const char* p_name, const char* p_class_name, int p_argc, F2<R, a0, a1> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 2) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 2 - default_arg_count));
		} else if (args_given > 2) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 2 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 2 argument(s).", 2 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 2 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0], *args[1]); return var();
		} else {
			return static_func(*args[0], *args[1]);
		}
	}
};

template<typename R, typename a0, typename a1, typename a2>
class _StaticFuncBind_F3 : public StaticFuncBind {
	F3<R, a0, a1, a2> static_func;
public:
	_StaticFuncBind_F3(const char* p_name, const char* p_class_name, int p_argc, F3<R, a0, a1, a2> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 3) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 3 - default_arg_count));
		} else if (args_given > 3) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 3 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 3 argument(s).", 3 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 3 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0], *args[1], *args[2]); return var();
		} else {
			return static_func(*args[0], *args[1], *args[2]);
		}
	}
};

template<typename R, typename a0, typename a1, typename a2, typename a3>
class _StaticFuncBind_F4 : public StaticFuncBind {
	F4<R, a0, a1, a2, a3> static_func;
public:
	_StaticFuncBind_F4(const char* p_name, const char* p_class_name, int p_argc, F4<R, a0, a1, a2, a3> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 4) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 4 - default_arg_count));
		} else if (args_given > 4) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 4 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 4 argument(s).", 4 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 4 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0], *args[1], *args[2], *args[3]); return var();
		} else {
			return static_func(*args[0], *args[1], *args[2], *args[3]);
		}
	}
};

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
class _StaticFuncBind_F5 : public StaticFuncBind {
	F5<R, a0, a1, a2, a3, a4> static_func;
public:
	_StaticFuncBind_F5(const char* p_name, const char* p_class_name, int p_argc, F5<R, a0, a1, a2, a3, a4> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 5) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 5 - default_arg_count));
		} else if (args_given > 5) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 5 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 5 argument(s).", 5 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 5 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0], *args[1], *args[2], *args[3], *args[4]); return var();
		} else {
			return static_func(*args[0], *args[1], *args[2], *args[3], *args[4]);
		}
	}
};

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
class _StaticFuncBind_F6 : public StaticFuncBind {
	F6<R, a0, a1, a2, a3, a4, a5> static_func;
public:
	_StaticFuncBind_F6(const char* p_name, const char* p_class_name, int p_argc, F6<R, a0, a1, a2, a3, a4, a5> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 6) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 6 - default_arg_count));
		} else if (args_given > 6) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 6 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 6 argument(s).", 6 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 6 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]); return var();
		} else {
			return static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5]);
		}
	}
};

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
class _StaticFuncBind_F7 : public StaticFuncBind {
	F7<R, a0, a1, a2, a3, a4, a5, a6> static_func;
public:
	_StaticFuncBind_F7(const char* p_name, const char* p_class_name, int p_argc, F7<R, a0, a1, a2, a3, a4, a5, a6> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = p_argc;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {

		int default_arg_count = mi->get_default_arg_count();
		int args_given = (int)args.size();
		if (args_given + default_arg_count < 7) { /* Args not enough. */
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", 7 - default_arg_count));
		} else if (args_given > 7) { /* More args proveded.	*/
			if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 7 argument(s).");
			else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format( "expected minimum of %i argument(s) and maximum of 7 argument(s).", 7 - default_arg_count));
		}

		stdvec<var> default_args_copy;
		for (int i = 7 - args_given; i > 0 ; i--) {
			default_args_copy.push_back(mi->get_default_args()[default_arg_count - i]);
		}
		for (var& v : default_args_copy) args.push_back(&v);

		if constexpr (std::is_same<R, void>::value) {
			static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]); return var();
		} else {
			return static_func(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6]);
		}
	}
};

template<typename T, typename R>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M0<T, R> m,
		 stdvec<var> default_args = {}) {
		
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>( ), make_stdvec<VarTypeInfo>(), ret, false, default_args, 0 );
	return newptr<_MethodBind_M0<T, R>>(method_name, p_class_name, 0, m, mi);
}

template<typename T, typename R>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M0_c<T, R> m,
		 stdvec<var> default_args = {}) {
		
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>( ), make_stdvec<VarTypeInfo>(), ret, false, default_args, 0 );
	return newptr<_MethodBind_M0_c<T, R>>(method_name, p_class_name, 0, m, mi);
}

template<typename T, typename R, typename a0>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M1<T, R, a0> m,
		const String& name0, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0 ), make_stdvec<VarTypeInfo>(vt0), ret, false, default_args, 1 );
	return newptr<_MethodBind_M1<T, R, a0>>(method_name, p_class_name, 1, m, mi);
}

template<typename T, typename R, typename a0>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M1_c<T, R, a0> m,
		const String& name0, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0 ), make_stdvec<VarTypeInfo>(vt0), ret, false, default_args, 1 );
	return newptr<_MethodBind_M1_c<T, R, a0>>(method_name, p_class_name, 1, m, mi);
}

template<typename T, typename R, typename a0, typename a1>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M2<T, R, a0, a1> m,
		const String& name0, const String& name1, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1 ), make_stdvec<VarTypeInfo>(vt0, vt1), ret, false, default_args, 2 );
	return newptr<_MethodBind_M2<T, R, a0, a1>>(method_name, p_class_name, 2, m, mi);
}

template<typename T, typename R, typename a0, typename a1>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M2_c<T, R, a0, a1> m,
		const String& name0, const String& name1, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1 ), make_stdvec<VarTypeInfo>(vt0, vt1), ret, false, default_args, 2 );
	return newptr<_MethodBind_M2_c<T, R, a0, a1>>(method_name, p_class_name, 2, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M3<T, R, a0, a1, a2> m,
		const String& name0, const String& name1, const String& name2, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2), ret, false, default_args, 3 );
	return newptr<_MethodBind_M3<T, R, a0, a1, a2>>(method_name, p_class_name, 3, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M3_c<T, R, a0, a1, a2> m,
		const String& name0, const String& name1, const String& name2, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2), ret, false, default_args, 3 );
	return newptr<_MethodBind_M3_c<T, R, a0, a1, a2>>(method_name, p_class_name, 3, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M4<T, R, a0, a1, a2, a3> m,
		const String& name0, const String& name1, const String& name2, const String& name3, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3), ret, false, default_args, 4 );
	return newptr<_MethodBind_M4<T, R, a0, a1, a2, a3>>(method_name, p_class_name, 4, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M4_c<T, R, a0, a1, a2, a3> m,
		const String& name0, const String& name1, const String& name2, const String& name3, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3), ret, false, default_args, 4 );
	return newptr<_MethodBind_M4_c<T, R, a0, a1, a2, a3>>(method_name, p_class_name, 4, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M5<T, R, a0, a1, a2, a3, a4> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4), ret, false, default_args, 5 );
	return newptr<_MethodBind_M5<T, R, a0, a1, a2, a3, a4>>(method_name, p_class_name, 5, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M5_c<T, R, a0, a1, a2, a3, a4> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4), ret, false, default_args, 5 );
	return newptr<_MethodBind_M5_c<T, R, a0, a1, a2, a3, a4>>(method_name, p_class_name, 5, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M6<T, R, a0, a1, a2, a3, a4, a5> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5), ret, false, default_args, 6 );
	return newptr<_MethodBind_M6<T, R, a0, a1, a2, a3, a4, a5>>(method_name, p_class_name, 6, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M6_c<T, R, a0, a1, a2, a3, a4, a5> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5), ret, false, default_args, 6 );
	return newptr<_MethodBind_M6_c<T, R, a0, a1, a2, a3, a4, a5>>(method_name, p_class_name, 6, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M7<T, R, a0, a1, a2, a3, a4, a5, a6> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, const String& name6, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5); DECLARE_VAR_TYPE(vt6, a6);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5, name6 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5, vt6), ret, false, default_args, 7 );
	return newptr<_MethodBind_M7<T, R, a0, a1, a2, a3, a4, a5, a6>>(method_name, p_class_name, 7, m, mi);
}

template<typename T, typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
ptr<MethodBind> _bind_method(const char* method_name, const char* p_class_name, M7_c<T, R, a0, a1, a2, a3, a4, a5, a6> m,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, const String& name6, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5); DECLARE_VAR_TYPE(vt6, a6);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(method_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5, name6 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5, vt6), ret, false, default_args, 7 );
	return newptr<_MethodBind_M7_c<T, R, a0, a1, a2, a3, a4, a5, a6>>(method_name, p_class_name, 7, m, mi);
}

template<typename R>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F0<R> f,
		 stdvec<var> default_args = {}) {
		
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>( ), make_stdvec<VarTypeInfo>(), ret, true, default_args, 0 );
	return newptr<_StaticFuncBind_F0<R>>(func_name, p_class_name, 0, f, mi);
}

template<typename R, typename a0>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F1<R, a0> f,
		const String& name0, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0 ), make_stdvec<VarTypeInfo>(vt0), ret, true, default_args, 1 );
	return newptr<_StaticFuncBind_F1<R, a0>>(func_name, p_class_name, 1, f, mi);
}

template<typename R, typename a0, typename a1>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F2<R, a0, a1> f,
		const String& name0, const String& name1, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1 ), make_stdvec<VarTypeInfo>(vt0, vt1), ret, true, default_args, 2 );
	return newptr<_StaticFuncBind_F2<R, a0, a1>>(func_name, p_class_name, 2, f, mi);
}

template<typename R, typename a0, typename a1, typename a2>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F3<R, a0, a1, a2> f,
		const String& name0, const String& name1, const String& name2, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2), ret, true, default_args, 3 );
	return newptr<_StaticFuncBind_F3<R, a0, a1, a2>>(func_name, p_class_name, 3, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F4<R, a0, a1, a2, a3> f,
		const String& name0, const String& name1, const String& name2, const String& name3, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3), ret, true, default_args, 4 );
	return newptr<_StaticFuncBind_F4<R, a0, a1, a2, a3>>(func_name, p_class_name, 4, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F5<R, a0, a1, a2, a3, a4> f,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3, name4 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4), ret, true, default_args, 5 );
	return newptr<_StaticFuncBind_F5<R, a0, a1, a2, a3, a4>>(func_name, p_class_name, 5, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F6<R, a0, a1, a2, a3, a4, a5> f,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5), ret, true, default_args, 6 );
	return newptr<_StaticFuncBind_F6<R, a0, a1, a2, a3, a4, a5>>(func_name, p_class_name, 6, f, mi);
}

template<typename R, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
ptr<StaticFuncBind> _bind_static_func(const char* func_name, const char* p_class_name, F7<R, a0, a1, a2, a3, a4, a5, a6> f,
		const String& name0, const String& name1, const String& name2, const String& name3, const String& name4, const String& name5, const String& name6, stdvec<var> default_args = {}) {
		DECLARE_VAR_TYPE(vt0, a0); DECLARE_VAR_TYPE(vt1, a1); DECLARE_VAR_TYPE(vt2, a2); DECLARE_VAR_TYPE(vt3, a3); DECLARE_VAR_TYPE(vt4, a4); DECLARE_VAR_TYPE(vt5, a5); DECLARE_VAR_TYPE(vt6, a6);
		DECLARE_VAR_TYPE(ret, R);
		ptr<MethodInfo> mi = newptr<MethodInfo>(func_name, make_stdvec<String>(name0, name1, name2, name3, name4, name5, name6 ), make_stdvec<VarTypeInfo>(vt0, vt1, vt2, vt3, vt4, vt5, vt6), ret, true, default_args, 7 );
	return newptr<_StaticFuncBind_F7<R, a0, a1, a2, a3, a4, a5, a6>>(func_name, p_class_name, 7, f, mi);
}


template<typename T, typename R>
using MVA = R(T::*)(stdvec<var*>&);

template<typename R>
using FVA = R(*)(stdvec<var*>&);

template<typename T, typename R>
class _MethodBind_MVA : public MethodBind {
	MVA<T, R> method;
public:
	_MethodBind_MVA(const char* p_name, const char* p_class_name, MVA<T, R> p_method, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = -1;
		method = p_method;
		mi = p_mi;
	}
	virtual var call(ptr<Object> self, stdvec<var*>& args) const override {
		if constexpr (std::is_same<R, void>::value) {
			(ptrcast<T>(self).get()->*method)(args); return var();
		} else {
			return (ptrcast<T>(self).get()->*method)(args);
		}
	}
	const MethodInfo* get_method_info() const { return mi.get(); }
};

template<typename R>
class _StaticFuncBind_FVA : public StaticFuncBind {
	FVA<R> static_func;
public:
	_StaticFuncBind_FVA(const char* p_name, const char* p_class_name, FVA<R> p_func, ptr<MethodInfo> p_mi) {
		name = p_name;
		class_name = p_class_name;
		argc = -1;
		static_func = p_func;
		mi = p_mi;
	}
	virtual var call(stdvec<var*>& args) const override {
		if constexpr (std::is_same<R, void>::value) {
			static_func(args); return var();
		} else {
			return static_func(args);
		}
	}
	const MethodInfo* get_method_info() const { return mi.get(); }
};

template<typename T, typename R>
ptr<MethodBind> _bind_va_method(const char* method_name, const char* p_class_name, MVA<T, R> m) {
	DECLARE_VAR_TYPE(ret, R);
	ptr<MethodInfo> mi = newptr<MethodInfo>( method_name, make_stdvec<String>(), make_stdvec<VarTypeInfo>(), ret, false, make_stdvec<var>(), -1);
	return newptr<_MethodBind_MVA<T, R>>(method_name, p_class_name, m, mi);
}

template<typename R>
ptr<StaticFuncBind> _bind_va_static_func(const char* func_name, const char* p_class_name, FVA<R> f) {
	DECLARE_VAR_TYPE(ret, R);
	ptr<MethodInfo> mi = newptr<MethodInfo>( func_name, make_stdvec<String>(), make_stdvec<VarTypeInfo>(), ret, true, make_stdvec<var>(), -1);
	return newptr<_StaticFuncBind_FVA<R>>(func_name, p_class_name, f, mi);
}
} // namespace

#endif // NATIVE_BIND_GEN_H

//include "_object.h"
//include "_string.h"

#define DEFVAL(m_val) m_val
#define DEFVALUES(...) make_stdvec<var>(__VA_ARGS__)
#define PARAMS(...) __VA_ARGS__

#define BIND_METHOD(m_name, m_method, ...)    p_native_classes->bind_data(_bind_method(m_name, get_class_name_s(), m_method, __VA_ARGS__))
#define BIND_METHOD_VA(m_name, m_method)      p_native_classes->bind_data(_bind_va_method(m_name, get_class_name_s(), m_method))
#define BIND_STATIC_FUNC(m_name, m_func, ...) p_native_classes->bind_data(_bind_static_func(m_name, get_class_name_s(), m_func, __VA_ARGS__))
#define BIND_STATIC_FUNC_VA(m_name, m_func)   p_native_classes->bind_data(_bind_va_static_func(m_name, get_class_name_s(), m_func))
#define BIND_MEMBER(m_name, m_member, ...)    p_native_classes->bind_data(_bind_member(m_name, get_class_name_s(), m_member, __VA_ARGS__))
#define BIND_STATIC_MEMBER(m_name, m_member)  p_native_classes->bind_data(_bind_static_member(m_name, get_class_name_s(), m_member))
#define BIND_CONST(m_name, m_const)           p_native_classes->bind_data(_bind_static_const(m_name, get_class_name_s(), m_const))
#define BIND_ENUM(m_name, ...)                p_native_classes->bind_data(_bind_enum(m_name, get_class_name_s(), __VA_ARGS__));
#define BIND_ENUM_VALUE(m_name, m_value)      p_native_classes->bind_data(newptr<EnumValueBind>(m_name, get_class_name_s(), m_value, newptr<EnumValueInfo>(m_name, m_value)));


namespace varh {
typedef ptr<Object>(*__constructor_f)();

class NativeClasses {
	struct ClassEntries {
		String class_name;
		String parent_class_name;
		__constructor_f __constructor = nullptr;
		const StaticFuncBind* __initializer = nullptr;
		stdhashtable<size_t, ptr<BindData>> bind_data;
	};

private:
	NativeClasses() {}
	static NativeClasses* _singleton;
	stdhashtable<size_t, ClassEntries> classes;

public:
	static NativeClasses* singleton();
	static void _set_singleton(NativeClasses* p_native_classes);
	static void cleanup();

	void bind_data(ptr<BindData> p_bind_data);
	void set_inheritance(const String& p_class_name, const String& p_parent_class_name);
	void set_constructor(const String& p_class_name, __constructor_f p__constructor);

	ptr<BindData> get_bind_data(const String& p_class_name, const String& attrib);
	ptr<BindData> find_bind_data(const String& p_class_name, const String& attrib);
	const MemberInfo* get_member_info(const String& p_class_name, const String& attrib);
	String get_inheritance(const String& p_class_name);
	bool is_class_registered(const String& p_class_name);
	const stdvec<const BindData*> get_bind_data_list(const String& p_class_name);
	const stdvec<const MemberInfo*> get_member_info_list(const String& p_class_name);

	ptr<Object> _new(const String& p_class_name);
	const StaticFuncBind* get_initializer(const String& p_class_name);
	ptr<Object> construct(const String& p_class_name, stdvec<var>& p_args = stdvec<var>());

	template<typename T>
	void register_class() {
		set_inheritance(T::get_class_name_s(), T::get_parent_class_name_s());
		set_constructor(T::get_class_name_s(), &T::__constructor);
		T::_bind_data(this);
	}

	template<typename T>
	void unregister_class() {
		throw "TODO:";
	}

};


}

#endif // NATIVE_CLASSES_H

#ifndef _ITERATOR_H
#define _ITERATOR_H

// DO NOT INCLUDE THIS AS IT WON'T WORK SINCE IT HAS DEPENDANCY IN _var.h
// INCLUDE _var.h INSETEAD. IT'S DESIGNED LIKE THIS TO GENERATE SINGLE HEADER.

namespace varh {

class _Iterator_String : public Object {
	REGISTER_CLASS(_Iterator_String, Object) {
		BIND_METHOD("__iter_has_next", &_Iterator_String::__iter_has_next);
		BIND_METHOD("__iter_next", &_Iterator_String::__iter_next);
	}
	size_t _it = 0;
	const String* _str_data = nullptr;

public:
	_Iterator_String() {}
	_Iterator_String(const String* p_str) : _str_data(p_str) {}

	bool __iter_has_next() override {
		return _it != _str_data->size();
	}

	var __iter_next() override {
		return String(_str_data->operator[](_it++));
	}
};

class _Iterator_Array : public Object {
	REGISTER_CLASS(_Iterator_Array, Object) {
		BIND_METHOD("__iter_has_next", &_Iterator_Array::__iter_has_next);
		BIND_METHOD("__iter_next", &_Iterator_Array::__iter_next);
	}
	stdvec<var>::iterator _it;
	stdvec<var>* _array_data = nullptr;
public:
	_Iterator_Array() {}
	_Iterator_Array(const Array* p_array) {
		_array_data = p_array->get_data();
		_it = _array_data->begin();
	}

	virtual bool __iter_has_next() override {
		return _it != _array_data->end();
	}
	virtual var __iter_next() override {
		return *(_it++);
	}

};

class _Map_KeyValue_Pair : public Object {
	REGISTER_CLASS(_Map_KeyValue_Pair, Object) {
		BIND_MEMBER("key", &_Map_KeyValue_Pair::key);
		BIND_MEMBER("value", &_Map_KeyValue_Pair::value);
	}
public:
	_Map_KeyValue_Pair() {}
	_Map_KeyValue_Pair(var p_key, var p_value) : key(p_key), value(p_value) {}

	var key, value;
};

class _Iterator_Map : public Object {
	REGISTER_CLASS(_Iterator_Map, Object) {
		BIND_METHOD("__iter_has_next", &_Iterator_Map::__iter_has_next);
		BIND_METHOD("__iter_next", &_Iterator_Map::__iter_next);
	}
	Map::_map_internal_t::iterator _it;
	Map::_map_internal_t* _map_data = nullptr;

public:

	_Iterator_Map() {}
	_Iterator_Map(const Map* p_map) {
		_map_data = p_map->get_data();
		_it = _map_data->begin();
	}

	virtual bool __iter_has_next() override {
		return _it != _map_data->end();
	}

	virtual var __iter_next() override {
		ptr<_Map_KeyValue_Pair>& ret = newptr<_Map_KeyValue_Pair>(_it->second.key, (_it)->second.value);
		_it++;
		return ret;
	}
};

}

#endif // _ITERATOR_H

// undefine all var.h macros defined in varcore.h
// this makes the user(carbon) independent of'em
#if defined(UNDEF_VAR_DEFINES)
#if !defined(VAR_H_HEADER_ONLY)

#undef func
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_PRINT
#undef DEBUG_BREAK
#undef VAR_ASSERT
#undef MISSED_ENUM_CHECK
#undef UNDEF_VAR_DEFINES

#endif
#endif

#endif // _VAR_H

//--------------- VAR_IMPLEMENTATION -------------------

#if defined(VAR_IMPLEMENTATION)

//include "_var.h"
#include <functional>

namespace varh {

var var::tmp;

std::ostream& operator<<(std::ostream& p_ostream, const String& p_str) {
	p_ostream << p_str.operator std::string();
	return p_ostream;
}
std::istream& operator>>(std::istream& p_istream, String& p_str) {
	p_istream >> *p_str._data;
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
	"BUG",
	"NULL_POINTER",
	"OPERATOR_NOT_SUPPORTED",
	"NOT_IMPLEMENTED",
	"ZERO_DIVISION",
	"TYPE_ERROR",
	"ATTRIBUTE_ERROR",
	"INVALID_ARG_COUNT",
	"INVALID_INDEX",
	//_ERROR_MAX_
};
MISSED_ENUM_CHECK(VarError::_ERROR_MAX_, 10);

std::string VarError::get_err_name(VarError::Type p_type) {
	return _error_names[p_type];
}

#define CHECK_METHOD_AND_ARGS()                                                                                                                         \
do {                                                                                                                                                    \
	if (has_member(p_method)) {                                                                                                                         \
		if (get_member_info(p_method)->get_type() != MemberInfo::METHOD)                                                                                \
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("member \"%s\" is not callable.", p_method.c_str()));                                   \
		const MethodInfo* mp = (MethodInfo*)get_member_info(p_method);                                                                                  \
		int arg_count = mp->get_arg_count();                                                                                                            \
		int default_arg_count = mp->get_default_arg_count();                                                                                            \
		if (arg_count != -1) {                                                                                                                          \
			if (p_args.size() + default_arg_count < arg_count) { /* Args not enough. */                                                                 \
				if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at exactly %i argument(s).", arg_count)); \
				else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", arg_count - default_arg_count));      \
			} else if (p_args.size() > arg_count) { /* More args proveded.    */                                                                           \
				if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at exactly %i argument(s).", arg_count)); \
				else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format(                                                                           \
					"expected minimum of %i argument(s) and maximum of %i argument(s).", arg_count - default_arg_count, arg_count));                    \
			}                                                                                                                                           \
		}                                                                                                                                               \
		for (int j = 0; j < mp->get_arg_types().size(); j++) {                                                                                          \
			if (mp->get_arg_types()[j] == VarTypeInfo(var::VAR)) continue; /* can't be _NULL. */                                                        \
			if (p_args.size() == j) break; /* rest are default args. */                                                                                 \
			if (mp->get_arg_types()[j] != VarTypeInfo(p_args[j]->get_type(), p_args[j]->get_type_name().c_str()))                                       \
				THROW_VARERROR(VarError::TYPE_ERROR, String::format(                                                                                    \
					"expected type %s at argument %i.", var::get_type_name_s(mp->get_arg_types()[j].type), j));                                         \
		}                                                                                                                                               \
	} else {                                                                                                                                            \
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" not exists on base %s.", p_method.c_str(), get_class_name_s()));     \
	}                                                                                                                                                   \
} while (false)

#define MEMBER_INFO_IMPLEMENTATION(m_type)                                                                                                \
bool m_type::has_member(const String& p_member) {                                                                                         \
	return get_member_info_list().find(p_member) != get_member_info_list().end();                                                         \
}                                                                                                                                         \
const MemberInfo* m_type::get_member_info(const String& p_member) {                                                                       \
	if (!has_member(p_member))                                                                                                            \
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" not exists on base " #m_type ".", p_member.c_str()));  \
	const stdmap<String, const MemberInfo*>& member_info = get_member_info_list();                                                        \
	return member_info.at(p_member);                                                                                                      \
}                                                                                                                                         \
const stdmap<String, const MemberInfo*>& m_type::get_member_info_list()

	// TODO: eventhought the method should be alive all the time it needs to be cleaned
	//       delete or use shared pointers.
#define MEMBER_INFO(m_name, ...) { m_name,      new MethodInfo(m_name, __VA_ARGS__) }

// String -----------------------------------------------

MEMBER_INFO_IMPLEMENTATION(String) {
	static const stdmap<String, const MemberInfo*> member_info = {
		MEMBER_INFO("size",                                               var::INT),
		MEMBER_INFO("length",                                             var::INT),
		MEMBER_INFO("to_int",                                             var::INT),
		MEMBER_INFO("to_float",                                           var::FLOAT),
		MEMBER_INFO("get_line",   {"line"}, {var::INT},                   var::STRING),
		MEMBER_INFO("hash",                                               var::INT),
		MEMBER_INFO("substr",     {"start", "end"}, {var::INT, var::INT}, var::STRING),
		MEMBER_INFO("endswith",   {"what"}, {var::STRING},                var::BOOL),
		MEMBER_INFO("startswith", {"what"}, {var::STRING},                var::BOOL),
	};
	return member_info;
}

var String::call_method(const String& p_method, const stdvec<var*>& p_args) {

	CHECK_METHOD_AND_ARGS();

	switch (p_method.const_hash()) {
		case "size"_hash:        // [[fallthrough]]
		case "length"_hash:      return (int64_t)size();
		case "to_int"_hash:     return to_int();
		case "to_float"_hash:   return to_float();
		case "get_line"_hash:   return get_line(p_args[0]->operator int64_t());
		case "hash"_hash:       return (int64_t)hash();
		case "substr"_hash:     return substr((size_t)p_args[0]->operator int64_t(), (size_t)p_args[1]->operator int64_t());
		case "endswith"_hash:   return endswith(p_args[0]->operator String());
		case "startswith"_hash: return startswith(p_args[0]->operator String());
	}
	// TODO: more.
	THROW_VARERROR(VarError::BUG, "can't reach here.");
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
	const char* source = _data->c_str();
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
	return _data->substr(p_start, p_end - p_start);
}
bool String::endswith(const String& p_str) const {
	if (p_str.size() > _data->size()) return false;
	for (size_t i = 1; i <= p_str.size(); i++) {
		if ((*_data)[_data->size() - i] != p_str[p_str.size() - i]) {
			return false;
		}
	}
	return true;
}
bool String::startswith(const String& p_str) const {
	if (p_str.size() > _data->size()) return false;
	for (size_t i = 0; i < p_str.size(); i++) {
		if ((*_data)[i] != p_str[i]) {
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
		MEMBER_INFO("size",                                   var::INT),
		MEMBER_INFO("empty",                                  var::BOOL),
		MEMBER_INFO("push_back", {"element"}, {var::VAR},     var::_NULL),
		MEMBER_INFO("pop_back",                               var::_NULL),
		MEMBER_INFO("append",    {"element"}, {var::VAR},     var::ARRAY),
		MEMBER_INFO("pop",                                    var::VAR),
		MEMBER_INFO("clear",                                  var::_NULL),
		MEMBER_INFO("at",        {"index"}, {var::INT},       var::VAR),
		MEMBER_INFO("resize",    {"size"}, {var::INT},        var::_NULL),
		MEMBER_INFO("reserve",   {"size"}, {var::INT},        var::_NULL),
	};
	return member_info;
}

var Array::call_method(const String& p_method, const stdvec<var*>& p_args) {
	CHECK_METHOD_AND_ARGS();
	switch (p_method.const_hash()) {
		case "size"_hash:      return (int64_t)size();
		case "empty"_hash:     return empty();
		case "push_back"_hash: { push_back(*p_args[0]); return var(); }
		case "pop_back"_hash: { pop_back(); return var(); }
		case "append"_hash:    return append(*p_args[0]);
		case "pop"_hash:       return pop();
		case "clear"_hash: { clear(); return var(); }
		case "insert"_hash: { insert(*p_args[0], *p_args[1]); return var(); }
		case "at"_hash:        return at(p_args[0]->operator int64_t());
		case "resize"_hash: { resize(p_args[0]->operator int64_t()); return var(); }
		case "reserve"_hash: { reserve(p_args[0]->operator int64_t()); return var(); }
	}
	// TODO: add more.
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

Array::Array(const stdvec<var>& p_data) {
	_data = newptr<stdvec<var>>();
	for (const var& v : p_data) _data->push_back(v);
}

String Array::to_string() const {
	// TODO: if the array contains itself it'll stack overflow.
	std::stringstream ss;
	ss << "[ ";
	for (unsigned int i = 0; i < _data->size(); i++) {
		ss << _data->operator[](i).to_string();
		if (i != _data->size() - 1) ss << ", ";
		else ss << " ";
	}
	ss << "]";
	return ss.str();
}

var Array::pop() { var ret = this->operator[](size() - 1); pop_back(); return ret; }

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
		MEMBER_INFO("size",                           var::INT),
		MEMBER_INFO("empty",                          var::BOOL),
		MEMBER_INFO("insert", {"what"}, {var::VAR},   var::_NULL),
		MEMBER_INFO("clear",                          var::_NULL),
		MEMBER_INFO("has",    {"what"}, {var::VAR},   var::BOOL),
	};
	return member_info;
}

var Map::call_method(const String& p_method, const stdvec<var*>& p_args) {
	CHECK_METHOD_AND_ARGS();
	switch (p_method.const_hash()) {
		case "size"_hash:   return (int64_t)size();
		case "empty"_hash:  return empty();
		case "insert"_hash: insert(*p_args[0], *p_args[1]); return var();
		case "clear"_hash:  clear(); return var();
		case "has"_hash:    return has(*p_args[0]);
	}
	// TODO: more.
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

Map::Map() {
	_data = newptr<_map_internal_t>();
}
Map::Map(const ptr<_map_internal_t>& p_data) {
	_data = p_data;
}
Map::Map(const Map& p_copy) {
	_data = p_copy._data;
}

String Map::to_string() const {
	std::stringstream ss;
	ss << "{ ";
	for (_map_internal_t::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (it != (*_data).begin()) ss << ", ";
		ss << it->second.key.to_string() << " : " << it->second.value.to_string();
	}
	ss << " }";
	return ss.str();
}

Map Map::copy(bool p_deep) const {
	Map ret;
	for (_map_internal_t::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (p_deep) {
			ret[(int64_t)it->first] = it->second.value.copy(true);
		} else {
			ret[(int64_t)it->first] = it->second.value;
		}
	}
	return ret;
}

// TODO: error message.
#define _INSERT_KEY_IF_HAVENT(m_key)                            \
	_map_internal_t::iterator it = (*_data).find(m_key.hash()); \
	if (it == _data->end()) (*_data)[m_key.hash()].key = m_key

var Map::operator[](const var& p_key) const { _INSERT_KEY_IF_HAVENT(p_key); return (*_data).operator[](p_key.hash()).value;  }
var& Map::operator[](const var& p_key) { _INSERT_KEY_IF_HAVENT(p_key); return (*_data).operator[](p_key.hash()).value; }
var Map::operator[](const char* p_key) const { _INSERT_KEY_IF_HAVENT(var(p_key)); return (*_data).operator[](var(p_key).hash()).value; }
var& Map::operator[](const char* p_key) { _INSERT_KEY_IF_HAVENT(var(p_key)); return (*_data).operator[](var(p_key).hash()).value; }

Map::_map_internal_t::iterator Map::begin() const { return (*_data).begin(); }
Map::_map_internal_t::iterator Map::end() const { return (*_data).end(); }
Map::_map_internal_t::iterator Map::find(const var& p_key) const { return (*_data).find(p_key.hash()); }

void Map::insert(const var& p_key, const var& p_value) {
	(*_data).insert(std::pair<size_t, _KeyValue>(p_key.hash(), _KeyValue(p_key, p_value)));
}
bool Map::has(const var& p_key) const { return find(p_key) != end(); }
void Map::clear() { _data->clear(); }

bool Map::operator ==(const Map& p_other) const {
	if (size() != p_other.size())
		return false;
	for (_map_internal_t::iterator it_other = p_other.begin(); it_other != p_other.end(); it_other++) {
		_map_internal_t::iterator it_self = find(it_other->second.key);
		if (it_self == end()) return false;
		if (it_self->second.value != it_other->second.value) return false;

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
var& Object::operator[](const var& p_key) { THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "use __set_mapped() instead."); }

void Object::_bind_data(NativeClasses* p_native_classes) {
	BIND_METHOD("get_class_name", &Object::get_class_name);
	BIND_METHOD("get_parent_class_name", &Object::get_parent_class_name);
}
// call_method() should call it's parent if method not exists.
var Object::call_method(ptr<Object> p_self, const String& p_name, stdvec<var*>& p_args) {
	String class_name = p_self->get_class_name();
	String method_name = p_name;

	if (!NativeClasses::singleton()->is_class_registered(class_name)) {
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", class_name.c_str()));
	}

	ptr<BindData> bind_data = NativeClasses::singleton()->find_bind_data(class_name, p_name);
	if (!bind_data) {
		bind_data = NativeClasses::singleton()->find_bind_data(class_name, __call_method);
		if (bind_data->get_type() == BindData::METHOD) {
			var _method_name = p_name;
			var _args = Array(); for (var* v : p_args) _args.operator Array().append(*v);
			p_args = { &_method_name, &_args };
		} else {
			bind_data = nullptr;
		}
	}

	if (bind_data) {
		if (bind_data->get_type() == BindData::METHOD) {
			return ptrcast<MethodBind>(bind_data)->call(p_self, p_args);

		} else if (bind_data->get_type() == BindData::STATIC_FUNC) {
			return ptrcast<StaticFuncBind>(bind_data)->call(p_args);

		} else {
			THROW_VARERROR(VarError::TYPE_ERROR,
				String::format("attribute named \"%s\" on type %s is not callable.", method_name.c_str(), p_self->get_class_name()));
		}
	}

	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no method named \"%s\".", p_self->get_class_name(), method_name.c_str()));
}


var Object::get_member(ptr<Object> p_self, const String& p_name) {
	String class_name = p_self->get_class_name();
	String member_name = p_name;

	if (!NativeClasses::singleton()->is_class_registered(class_name)) {
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", class_name.c_str()));
	}

	ptr<BindData> bind_data = NativeClasses::singleton()->find_bind_data(class_name, member_name);
	if (bind_data) {
		if (bind_data->get_type() == BindData::MEMBER_VAR) {
			return ptrcast<PropertyBind>(bind_data)->get(p_self);
		} else if (bind_data->get_type() == BindData::STATIC_VAR) {
			return ptrcast<StaticPropertyBind>(bind_data)->get();
		} else if (bind_data->get_type() == BindData::STATIC_CONST) {
			return ptrcast<ConstantBind>(bind_data)->get();
		} else if (bind_data->get_type() == BindData::ENUM_VALUE) {
			return ptrcast<EnumValueBind>(bind_data)->get();

		} else {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("attribute named \"%s\" on type %s is not a property.", member_name.c_str(), p_self->get_class_name()));
		}
	}
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no member named \"%s\"", p_self->get_class_name(), member_name.c_str()));
}


void Object::set_member(ptr<Object> p_self, const String& p_name, var& p_value) {
	String class_name = p_self->get_class_name();
	String member_name = p_name;

	if (!NativeClasses::singleton()->is_class_registered(class_name)) {
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", class_name.c_str()));
	}

	ptr<BindData> bind_data = NativeClasses::singleton()->find_bind_data(class_name, member_name);
	if (bind_data) {
		if (bind_data->get_type() == BindData::MEMBER_VAR) {
			ptrcast<PropertyBind>(bind_data)->get(p_self) = p_value;
		} else if (bind_data->get_type() == BindData::STATIC_VAR) {
			ptrcast<StaticPropertyBind>(bind_data)->get() = p_value;

		} else if (bind_data->get_type() == BindData::STATIC_CONST) {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't assign a value to constant named \"%s\" on type \"%s\".", member_name.c_str(), p_self->get_class_name()));
		} else if (bind_data->get_type() == BindData::ENUM_VALUE) {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't assign a value to enum value named \"%s\" on type \"%s\".", member_name.c_str(), p_self->get_class_name()));
		} else {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("attribute named \"%s\" on type \"%s\" is not a property.", member_name.c_str(), p_self->get_class_name()));
		}
	}
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no member named \"%s\".", p_self->get_class_name(), member_name.c_str()));
}

const stdvec<const MemberInfo*> Object::get_member_info_list(const Object* p_instance) {
	if (p_instance) return NativeClasses::singleton()->get_member_info_list(p_instance->get_class_name());
	else return NativeClasses::singleton()->get_member_info_list(Object::get_class_name_s());
}
const MemberInfo* Object::get_member_info(const Object* p_instance, const String& p_member) {
	if (p_instance) return NativeClasses::singleton()->get_member_info(p_instance->get_class_name(), p_member);
	else return NativeClasses::singleton()->get_member_info(Object::get_class_name_s(), p_member);
}


// TODO: change these methods as static and call with an instance -> the base name could be found with self->get_class_name();

#define _OBJ_THROW_NOT_IMPL(m_name)\
	THROW_VARERROR(VarError::NOT_IMPLEMENTED, String("operator " #m_name " not implemented on base ") + get_class_name() + ".")

var Object::__call(stdvec<var>& p_vars) { _OBJ_THROW_NOT_IMPL(__call); }
var Object::operator()(stdvec<var>& p_vars) { return __call(p_vars); }

var Object::__get_mapped(const var& p_key) const { _OBJ_THROW_NOT_IMPL(__get_mapped()); }
void Object::__set_mapped(const var& p_key, const var& p_val) { _OBJ_THROW_NOT_IMPL(__set_mapped()); }
int64_t Object::__hash() const { _OBJ_THROW_NOT_IMPL(__hash()); }

var  Object::__iter_begin() { _OBJ_THROW_NOT_IMPL(__iter_begin()); }
bool Object::__iter_has_next() { _OBJ_THROW_NOT_IMPL(__iter_has_next()); }
var  Object::__iter_next() { _OBJ_THROW_NOT_IMPL(__iter_next()); }

var Object::__add(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__add()); }
var Object::__sub(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__sub()); }
var Object::__mul(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__mul()); }
var Object::__div(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__div()); }

var& Object::__add_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__add_eq()); }
var& Object::__sub_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__sub_eq()); }
var& Object::__mul_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__mul_eq()); }
var& Object::__div_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__div_eq()); }

bool Object::__gt(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__gt()); }
bool Object::__lt(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__lt()); }
bool Object::__eq(const var& p_other) const {
	if (p_other.get_type() != var::OBJECT) return false;
	return this == p_other.operator varh::ptr<varh::Object>().get();
}
#undef _OBJ_THROW_NOT_IMPL

// var -----------------------------------------------

size_t var::hash() const {
	switch (type) {
		case _NULL:
			THROW_VARERROR(VarError::NULL_POINTER, "");
		case BOOL:   return std::hash<bool>{}(_data._bool);
		case INT:    return std::hash<int64_t>{}(_data._int);
		case FLOAT:  return std::hash<double>{}(_data._float);
		case STRING: return _data._string.hash();
		case ARRAY:
		case MAP:
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("key of typt %s is unhashable.", get_type_name().c_str()));
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

bool var::is_hashable(var::Type p_type) {
	switch (p_type) {
		case _NULL:  return false;
		case BOOL:
		case INT:
		case FLOAT:
		case STRING:
			return true;
		case ARRAY:
		case MAP:
			return false;
		case OBJECT:
			return true;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

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
			return *this;
		case ARRAY: return _data._arr.copy(p_deep);
		case MAP: return _data._map.copy(p_deep);
		case OBJECT: return _data._obj->copy(p_deep);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_VARERROR(VarError::BUG, "can't reach here.");
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

var::var(size_t p_int) {
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

var::var(const Array& p_array) {
	type = ARRAY;
	new(&_data._arr) Array(p_array);
}

var::var(const Map& p_map) {
	type = MAP;
	new(&_data._map) Map(p_map);
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)                                                                      \
var var::operator m_op () {                                                                             \
	switch (type) {                                                                                     \
		case INT:  return m_op _data._int;                                                              \
		case FLOAT: return m_op _data._float;                                                           \
		default: THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED,                                        \
			String::format("operator " #m_op " not supported on base %s.", get_type_name().c_str()));   \
	}                                                                                                   \
	return var();                                                                                       \
}

#define VAR_OP_POST_INCR_DECR(m_op)                                                                     \
var var::operator m_op(int) {                                                                           \
	switch (type) {                                                                                     \
		case INT: return _data._int m_op;                                                               \
		case FLOAT: return _data._float m_op;                                                           \
		default: THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED,                                          \
			String::format("operator " #m_op " not supported on base %s.", get_type_name().c_str()));   \
	}                                                                                                   \
	return var();                                                                                       \
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
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, String::format("operator[] not supported on base %s", get_type_name()));
}

var var::__get_mapped(const var& p_key) const {
	switch (type) {
		case STRING: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			int64_t index = p_key;
			return String(_data._string[index]);
		} break;
		case ARRAY: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			int64_t index = p_key;
			return _data._arr[index];
		} break;
		case MAP:
			if (!_data._map.has(p_key)) THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("key %s does not exists on base Map.", p_key.to_string()));
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->__get_mapped(p_key);
	}
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, String::format("operator[] not supported on base %s", get_type_name()));
}

void var::__set_mapped(const var& p_key, const var& p_value) {
	switch (type) {
		case STRING: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			if (p_value.get_type() != var::STRING) THROW_VARERROR(VarError::TYPE_ERROR, "expected a string value to assign");
			if (p_value.operator String().size() != 1) THROW_VARERROR(VarError::TYPE_ERROR, "expected a string of size 1 to assign");
			_data._string[p_key.operator int64_t()] = p_value.operator String()[0];
		} break;
		case ARRAY: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			_data._arr[p_key.operator int64_t()] = p_value;
		} break;
		case MAP:
			_data._map[p_key] = p_value;
		case OBJECT:
			_data._obj->__set_mapped(p_key, p_value);
	}
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, String::format("operator[] not supported on base %s", get_type_name()));
}

var var::__iter_begin() {
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not iterable.");
		case var::INT:    THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "integer is not iterable.");
		case var::FLOAT:  THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "float is not iterable.");
		case var::STRING: return newptr<_Iterator_String>(&_data._string);
		case var::ARRAY: return newptr<_Iterator_Array>(&_data._arr);
		case var::MAP: return newptr<_Iterator_Map>(&_data._map);
		case var::OBJECT: return _data._obj.get()->__iter_begin();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var var::__call_internal(stdvec<var*>& p_args) {
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not callable.");
		case var::INT:    THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "integer is not callable.");
		case var::FLOAT:  THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "float is not callable.");
		case var::STRING: THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "String is not callable.");
		case var::ARRAY:  THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "Array is not callable.");
		case var::MAP:    THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "Map is not callable.");
		case var::OBJECT: return _data._obj.get()->__call(p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var var::call_method_internal(const String& p_method, stdvec<var*>& p_args) {

	// check var methods.
	switch (p_method.const_hash()) {
		case "to_string"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return to_string();
		case "copy"_hash:
			if (p_args.size() >= 2) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at maximum 1 argument.");
			if (p_args.size() == 0) return copy();
			return copy(p_args[0]->operator bool());
		case "hash"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return (int64_t)hash();
		case "get_type_name"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return get_type_name();

		// operators.
		case "__iter_begin"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return __iter_begin();
		case "__get_mapped"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return __get_mapped(*p_args[0]);
		case "__set_mapped"_hash:
			if (p_args.size() != 2) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			__set_mapped(*p_args[0], *p_args[1]); return var();
		case "__add"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator +(*p_args[0]);
		case "__sub"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator -(*p_args[0]);
		case "__mul"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator *(*p_args[0]);
		case "__div"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator /(*p_args[0]);
		case "__add_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator +=(*p_args[0]);
		case "__sub_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator -=(*p_args[0]);
		case "__mul_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator *=(*p_args[0]);
		case "__div_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator /=(*p_args[0]);
		case "__gt"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator >(*p_args[0]);
		case "__lt"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator <(*p_args[0]);
		case "__eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator ==(*p_args[0]);

		case "__call"_hash:
			return __call_internal(p_args);
	}

	// type methods.
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("boolean has no attribute \"%s\".", p_method.c_str()));
		case var::INT:    THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("int has no attribute \"%s\".", p_method.c_str()));
		case var::FLOAT:  THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("float has no attribute \"%s\".", p_method.c_str()));
		case var::STRING: return _data._string.call_method(p_method, p_args);
		case var::ARRAY:  return _data._arr.call_method(p_method, p_args);
		case var::MAP:    return _data._map.call_method(p_method, p_args);
		case var::OBJECT: return Object::call_method(_data._obj, p_method, p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var var::get_member(const String& p_name) {
	switch (type) {
		case var::_NULL:   THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:    THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("boolean has no attribute \"%s\".", p_name.c_str()));
		case var::INT:     THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("int has no attribute \"%s\".", p_name.c_str()));
		case var::FLOAT:   THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("float has no attribute \"%s\".", p_name.c_str()));
		case var::STRING:
		case var::ARRAY:
		case var::MAP:
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" does not exists on base %s.", p_name.c_str(), get_type_name().c_str()));
		case var::OBJECT: return Object::get_member(_data._obj, p_name);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

void var::set_member(const String& p_name, var& p_value) {
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("boolean has no attribute \"%s\".", p_name.c_str()));
		case var::INT:    THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("int has no attribute \"%s\".", p_name.c_str()));
		case var::FLOAT:  THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("float has no attribute \"%s\".", p_name.c_str()));
		case var::STRING:
		case var::ARRAY:
		case var::MAP:
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" does not exists on \"%s\".", p_name.c_str(), get_type_name().c_str()));
		case var::OBJECT: Object::set_member(_data._obj, p_name, p_value);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

const MemberInfo* var::get_member_info(const String& p_name) const {
	if (type == OBJECT) return Object::get_member_info(_data._obj.get(), p_name);
	else return var::get_member_info_s(type, p_name);
}

const MemberInfo* var::get_member_info_s(var::Type p_type, const String& p_name) {
	switch (p_type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return nullptr;
		case var::STRING: return String::get_member_info(p_name);
		case var::ARRAY:  return Array::get_member_info(p_name);
		case var::MAP:    return Map::get_member_info(p_name);
		case var::OBJECT: return Object::get_member_info(nullptr, p_name);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

const stdvec<const MemberInfo*> var::get_member_info_list() const {
	if (type == OBJECT) return Object::get_member_info_list(_data._obj.get());
	else return get_member_info_list_s(type);
}

stdvec<const MemberInfo*> var::get_member_info_list_s(var::Type p_type) {
	stdvec<const MemberInfo*> ret;
	const stdmap<String, const MemberInfo*>* list = nullptr;
	switch (p_type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return ret;
		case var::STRING:
			list = &String::get_member_info_list();
			// [[fallthrough]]
		case var::ARRAY:
			list = &Array::get_member_info_list();
			// [[fallthrough]]
		case var::MAP:
			list = &Map::get_member_info_list();
			{
				stdmap<String, const MemberInfo*>::const_iterator it = list->begin();
				while (it != list->end()) {
					const MemberInfo* mi = (*it).second;
					ret.push_back(mi);
					it++;
				}
			}
			return ret;
		case var::OBJECT: {
			return Object::get_member_info_list(nullptr);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL:  return false;
		case BOOL:   return _data._bool;
		case INT:    return _data._int != 0;
		case FLOAT:  return _data._float != 0;
		case STRING: return _data._string.size() != 0;
		case ARRAY:  return !_data._arr.empty();
		case MAP:    return !_data._map.empty();
		case OBJECT: return _data._obj.operator bool();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"bool\".", get_type_name().c_str()));
}

var::operator int64_t() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		//case STRING: return  _data._string.to_int(); // throws std::exception
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"int\".", get_type_name().c_str()));
}

var::operator double() const {
	switch (type) {
		case BOOL: return (double)_data._bool;
		case INT: return (double)_data._int;
		case FLOAT: return _data._float;
		//case STRING: return  _data._string.to_float();
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"float\".", get_type_name().c_str()));
}

var::operator String() const {
	if (type != STRING)
		THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"String\".", get_type_name().c_str()));
	return _data._string;
}

String var::to_string() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return String(_data._int);
		case FLOAT: return String(_data._float);
		case STRING: return _data._string;
		case ARRAY: return _data._arr.to_string();
		case MAP: return _data._map.to_string();
		case OBJECT: return _data._obj->to_string();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var::operator Array() const {
	if (type == ARRAY) {
		return _data._arr;
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"Array\".", get_type_name().c_str()));
}

var::operator Map() const {
	if (type == MAP) {
		return _data._map;
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"Map\".", get_type_name().c_str()));
}

var::operator ptr<Object>() const {
	if (type == OBJECT) {
		return _data._obj;
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"Object\".", get_type_name().c_str()));
}

/* operator overloading */
		/* comparison */

#define THROW_OPERATOR_NOT_SUPPORTED(m_op)                                                         \
do {                                                                                               \
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED,                                                \
		String::format("operator \"" STR(m_op) "\" not supported on operands \"%s\" and \"%s\".",  \
			get_type_name().c_str(), p_other.get_type_name().c_str())                              \
	);                                                                                             \
} while(false)

bool var::operator==(const var& p_other) const {

	// TODO: if other.type == object and it has overload operator.

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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	return false; // different types.
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(<);
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(>);
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
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
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__sub(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
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
				case ARRAY: {
					Array ret;
					ret._data->reserve(ret._data->size() * _data._int);
					for (int64_t i = 0; i < _data._int; i++) {
						ret._data->insert(ret._data->end(), p_other._data._arr.begin(), p_other._data._arr.end());
					}
					return ret;
					break;
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
		case ARRAY:
			if (p_other.type == INT) {
				Array ret;
				ret._data->reserve(_data._arr.size() * p_other._data._int);
				for (int64_t i = 0; i < p_other._data._int; i++) {
					ret._data->insert(ret._data->end(), _data._arr.begin(), _data._arr.end());
				}
				return ret;
			}
			break;
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(*);
}

var var::operator /(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (double)_data._bool  / p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._int         / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._int         / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (double)_data._int / p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._float / p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__div(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(%);
}

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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
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
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__sub_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(-=);
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int64_t)_data._bool    * (int64_t)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int64_t)_data._bool    * p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool     * p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int64_t)_data._int * (int64_t)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int  * p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int  * p_other._data._float;      return *this;
				case STRING: {
					String self;
					for (int64_t i = 0; i < _data._int; i++) {
						self += p_other._data._string;
					}
					type = STRING; new(&_data._string) String(self); return *this;
				}
				case ARRAY: {
					Array self;
					for (int64_t i = 0; i < _data._int; i++) {
						self._data->insert(self.end(), p_other._data._arr.begin(), p_other._data._arr.end());
					}
					clear();
					type = ARRAY; _data._arr = self; return *this;
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
		case ARRAY: {
			if (p_other.type == INT) {
				_data._arr.reserve(_data._arr.size() * p_other._data._int);
				for (int64_t i = 0; i < p_other._data._int -1; i++) {
					_data._arr._data->insert(_data._arr.end(), _data._arr.begin(), _data._arr.end());
				}
				return *this;
			}
		}
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(*=);
}

var& var::operator/=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / (int)p_other._data._bool; return *this;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / p_other._data._int;       return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._bool / p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._int   = (int)   _data._int / (int)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._int   =         _data._int / p_other._data._int;        return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._int / p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._int;   return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
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
		case var::ARRAY:
			new(&_data._arr) Array(p_other._data._arr);
			break;
		case var::MAP:
			new(&_data._map) Map(p_other._data._map);
			break;
		case var::OBJECT:
			new(&_data._obj) ptr<Object>(p_other._data._obj);
			return;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
}

void var::clear_data() {
	switch (type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return;
		case var::STRING:
			_data._string.~String();
			return;
		case var::ARRAY:
			_data._arr.~Array();
			break;
		case var::MAP:
			_data._map.~Map();
			break;
		case var::OBJECT:
			_data._obj = nullptr;
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
}

String var::get_type_name() const {
	if (type == OBJECT) {
		return _data._obj->get_class_name();
	} else {
		return get_type_name_s(type);
	}
}

}


#if defined(UNDEF_VAR_DEFINES)
#undef func
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


//include "_var.h"

namespace varh {

NativeClasses* NativeClasses::_singleton = nullptr;

NativeClasses* NativeClasses::singleton() {
	if (_singleton == nullptr) _singleton = new NativeClasses();
	return _singleton;
}

void NativeClasses::_set_singleton(NativeClasses* p_native_classes) {
	if (_singleton == nullptr) { // share singleton across dynamic libraries.
		_singleton = p_native_classes;
	}
}

void NativeClasses::cleanup() {
	delete _singleton;
}

void NativeClasses::bind_data(ptr<BindData> p_bind_data) {
	String class_name = p_bind_data->get_class_name();
	String data_name = p_bind_data->get_name();

	ClassEntries& entries = classes[class_name.hash()];

	if (entries.bind_data[data_name.hash()] != nullptr) {
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, 
			String::format("entry \"%s\" already exists on class \"%s\".", p_bind_data->get_name(), p_bind_data->get_class_name())
		);
	}
	// check initializer.
	if (class_name == data_name) {
		if (p_bind_data->get_type() != BindData::STATIC_FUNC)
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" can't be the same as type name.", data_name.c_str()));
		const MethodInfo* mi = ptrcast<StaticFuncBind>(p_bind_data)->get_method_info();
		if (mi->get_return_type().type != var::_NULL) THROW_VARERROR(VarError::TYPE_ERROR, "constructor initializer must not return anything.");
		if (mi->get_arg_count() < 1 || mi->get_arg_types()[0].type != var::OBJECT) THROW_VARERROR(VarError::TYPE_ERROR, "constructor initializer must take the instance as the first argument.");
		entries.__initializer = ptrcast<StaticFuncBind>(p_bind_data).get();		
	}
	entries.bind_data[data_name.hash()] = p_bind_data;
}

ptr<BindData> NativeClasses::get_bind_data(const String& cls, const String& attrib) {
	if (!is_class_registered(cls))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" not registered on NativeClasses entries.", cls.c_str()));
	return classes[cls.hash()].bind_data[attrib.hash()];
}

ptr<BindData> NativeClasses::find_bind_data(const String& cls, const String& attrib) {
	String class_name = cls;
	while (class_name.size() != 0) {
		ptr<BindData> bind_data = NativeClasses::get_bind_data(class_name, attrib);
		if (bind_data != nullptr) return bind_data;
		class_name = NativeClasses::get_inheritance(class_name);
	}
	return nullptr;
}

const MemberInfo* NativeClasses::get_member_info(const String& p_class_name, const String& attrib) {
	return get_bind_data(p_class_name, attrib)->get_member_info();
}

void NativeClasses::set_inheritance(const String& p_class_name, const String& p_parent_class_name) {
	if (is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" already exists on NativeClasses entries.", p_class_name.c_str()));

	classes[p_class_name.hash()].class_name = p_class_name;
	classes[p_class_name.hash()].parent_class_name = p_parent_class_name;
}

void NativeClasses::set_constructor(const String& p_class_name, __constructor_f p__constructor) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" not registered on NativeClasses entries.", p_class_name.c_str()));
	classes[p_class_name.hash()].__constructor = p__constructor;
}

String NativeClasses::get_inheritance(const String& p_class_name) {
	if (classes[p_class_name.hash()].class_name.size() == 0)
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	return classes[p_class_name.hash()].parent_class_name;
}

bool NativeClasses::is_class_registered(const String& p_class_name) {
	return classes[p_class_name.hash()].class_name.size() != 0;
}

ptr<Object> NativeClasses::_new(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	return classes[p_class_name.hash()].__constructor();
}

const StaticFuncBind* NativeClasses::get_initializer(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	return classes[p_class_name.hash()].__initializer;
}

ptr<Object> NativeClasses::construct(const String& p_class_name, stdvec<var>& p_args) {
	ptr<Object> instance = _new(p_class_name);
	p_args.insert(p_args.begin(), instance);
	const StaticFuncBind* initializer = get_initializer(p_class_name);
	if (initializer) initializer->call(p_args);
	return instance;
}

const stdvec<const BindData*> NativeClasses::get_bind_data_list(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	stdvec<const BindData*> ret;
	stdhashtable<size_t, ptr<BindData>>& bind_data_list = classes[p_class_name.hash()].bind_data;
	stdhashtable<size_t, ptr<BindData>>::iterator it = bind_data_list.begin();
	while (it != bind_data_list.end()) {
		const BindData* bd = (*it).second.get();
		ret.push_back(bd);
		it++;
	}
	return ret;
}

const stdvec<const MemberInfo*> NativeClasses::get_member_info_list(const String& p_class_name) {
	if (!is_class_registered(p_class_name))
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", p_class_name.c_str()));
	stdvec<const MemberInfo*> ret;
	stdhashtable<size_t, ptr<BindData>>& bind_data_list = classes[p_class_name.hash()].bind_data;
	stdhashtable<size_t, ptr<BindData>>::iterator it = bind_data_list.begin();
	while (it != bind_data_list.end()) {
		const BindData* bd = (*it).second.get();
		ret.push_back(bd->get_member_info());
		it++;
	}
	return ret;
}

}


#endif // VAR_IMPLEMENTATION