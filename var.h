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

// SAMPLES:
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

#define STRCAT2(m_a, m_b) m_a##m_b
#define STRCAT3(m_a, m_b, m_c) m_a##m_b##m_c
#define STRCAT4(m_a, m_b, m_c, m_d) m_a##m_b##m_c##m_d

#define STR(m_mac) #m_mac
#define STRINGIFY(m_mac) STR(m_mac)
#define PLACE_HOLDER_MACRO

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

namespace varh {

class String;

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


class VarError : public std::exception {
public:
	enum Type {
		OK,
		NULL_POINTER,
		INVALID_INDEX,
		INVALID_KEY,
		INVALID_CASTING,
		INVALID_GET_NAME,
		INVALID_SET_VALUE,
		INVALID_ARGUMENT,
		INVALID_ARG_COUNT,

		NOT_IMPLEMENTED,
		OPERATOR_NOT_SUPPORTED,
		ZERO_DIVISION,

		BUG,

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
	

private:
	Type type = OK;
	std::string msg;
};
}

#endif // VARHCORE_H

namespace varh {

class var;
class Object;

class String {
public:

	// Constructors.
	String()                           {}
	String(const std::string& p_copy)  { _data = p_copy; }
	String(const char* p_copy)         { _data = p_copy; }
	String(const String& p_copy)       { _data = p_copy._data; }
	String(char p_char)                { _data = p_char; }
	explicit String(int p_i)                    { _data = std::to_string(p_i); }
	explicit String(int64_t p_i)                { _data = std::to_string(p_i); }
	explicit String(size_t p_i)                 { _data = std::to_string(p_i); }
	explicit String(double p_d)                 { _data = std::to_string(p_d); }
	~String()                          {}

	// reflection methods.
	var call_method(const String& p_method, const stdvec<var>& p_args);
	static const stdmap<String, const MemberInfo*>& get_member_info();
	static bool has_member(const String& p_member);
	static const MemberInfo* get_member(const String& p_member);

	static String format(const char* p_format, ...);
	int64_t to_int() const { return std::stoll(_data); } // TODO: this will throw std::exceptions
	double to_float() const { return std::stod(_data); }
	String get_line(uint64_t p_line) const;
	size_t hash() const { return std::hash<std::string>{}(_data); }
	size_t const_hash() const { return __const_hash(_data.c_str()); }

	String substr(size_t p_start, size_t p_end) const;
	bool endswith(const String& p_str) const;
	bool startswith(const String& p_str) const;
	// String strip(p_delemeter = " "); lstrip(); rstrip();
	// Array split(p_delimeter = " ");

	// operators.
	char operator[](int64_t p_index) const {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data[p_index];
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data[size() + p_index];
		throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error message.
	}
	char& operator[](int64_t p_index) {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data[p_index];
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data[size() + p_index];
		throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error message.
	}

	operator std::string() const                   { return _data; }
	// operator bool() {} don't implement this don't even delete
	
	bool operator==(const String & p_other) const  { return _data == p_other._data; }
	bool operator!=(const String & p_other) const  { return _data != p_other._data; }
	bool operator<(const String& p_other) const    { return _data < p_other._data; }

	String operator+(char p_c) const               { return _data + p_c; }
	String operator+(int p_i) const                { return _data + std::to_string(p_i); }
	String operator+(double p_d) const             { return _data + std::to_string(p_d); }
	String operator+(const char* p_cstr) const     { return _data + p_cstr; }
	String operator+(const String& p_other) const  { return _data + p_other._data; }
	// String operator+(var& p_obj) const          { TODO: }

	String& operator+=(char p_c)                   { _data += p_c;                 return *this; }
	String& operator+=(int p_i)                    { _data += std::to_string(p_i); return *this; }
	String& operator+=(double p_d)                 { _data += std::to_string(p_d); return *this; }
	String& operator+=(const char* p_cstr)         { _data += p_cstr;              return *this; }
	String& operator+=(const String& p_other)      { _data += p_other._data;       return *this; }
	// String& operator+(var& p_obj)               { TODO: }

	String& operator=(char p_c)                   { _data = p_c;                 return *this; }
	String& operator=(int p_i)                    { _data = std::to_string(p_i); return *this; }
	String& operator=(double p_d)                 { _data = std::to_string(p_d); return *this; }
	String& operator=(const char* p_cstr)         { _data = p_cstr;              return *this; }
	String& operator=(const String& p_other)      { _data = p_other._data;       return *this; }
	// String& operator=(var& p_obj)               { TODO: }

	// Wrappers.
	size_t size() const                   { return _data.size(); }
	const char* c_str() const             { return _data.c_str(); }
	String& append(const String& p_other) { _data.append(p_other); return *this; }

private:
	friend class var;
	friend std::ostream& operator<<(std::ostream& p_ostream, const String& p_str);
	friend std::istream& operator>>(std::istream& p_ostream, String& p_str);
	std::string _data;
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
	Array(const ptr<std::vector<var>>& p_data) {
		_data = p_data;
	}
	Array(const Array& p_copy) {
		_data = p_copy._data;
	}
	template <typename... Targs>
	Array(Targs... p_args) {
		_data = newptr<stdvec<var>>();
		_make_va_arg_array(p_args...);
	}

	std::vector<var>* get_data() {
		return _data.operator->();
	}
	std::vector<var>* get_data() const {
		return _data.operator->();
	}

	Array copy(bool p_deep = true) const;

	// reflection methods.
	var call_method(const String& p_method, const stdvec<var>& p_args);
	static const stdmap<String, const MemberInfo*>& get_member_info();
	static bool has_member(const String& p_member);
	static const MemberInfo* get_member(const String& p_member);

	// Wrappers.
	// TODO: throw all errors with VarError
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void push_back(const var& p_var) { _data->push_back(p_var); }
	void pop_back() { _data->pop_back(); }
	Array& append(const var& p_var) { push_back(p_var); return *this; }
	var& pop() { var& ret = this->operator[](size() - 1); pop_back(); return ret; } 
	var& operator[](int64_t p_index) const {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data->operator[](p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data->operator[](size() + p_index);
		throw VarError(VarError::INVALID_INDEX, "");
	}
	var& operator[](int64_t p_index) { 
		if (0 <= p_index && p_index < (int64_t)size())
			return _data->operator[](p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data->operator[](size() + p_index);
		throw VarError(VarError::INVALID_INDEX, "");
	}
	std::vector<var>::const_iterator begin() const { return (*_data).begin(); }
	std::vector<var>::const_iterator end() const { return (*_data).end(); }
	void clear() { (*_data).clear(); }
	var& at(int64_t p_index) {
		if (0 <= p_index && p_index < (int64_t)size())
			return (*_data).at(p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return (*_data).at(size() + p_index);
		throw VarError(VarError::INVALID_INDEX, "");
	}
	var& back() { return (*_data).back(); }
	var& front() { return (*_data).front(); }
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

#define MISSED_ENUM_CHECK(m_max_enum, m_max_value) \
    static_assert((int)m_max_enum == m_max_value, "MissedEnum: " STRINGIFY(m_max_enum) " != " STRINGIFY(m_value) \
        "\n\tat: " __FILE__ "(" STRINGIFY(__LINE__) ")")

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

#endif //VECTOR_H

#ifndef  MAP_H
#define  MAP_H

//include "varhcore.h"


namespace varh {

class var;
class String;

class Map {
public:
	// Mehtods.
	Map() {
		_data = std::make_shared<stdmap<var, var>>();
	}
	Map(const ptr<stdmap<var, var>>& p_data) {
		_data = p_data;
	}
	Map(const Map& p_copy) {
		_data = p_copy._data;
	}

	stdmap<var, var>* get_data() {
		return _data.operator->();
	}
	stdmap<var, var>* get_data() const {
		return _data.operator->();
	}

	Map copy(bool p_deep = true) const;

	// reflection methods.
	var call_method(const String& p_method, const stdvec<var>& p_args);
	static const stdmap<String, const MemberInfo*>& get_member_info();
	static bool has_member(const String& p_member);
	static const MemberInfo* get_member(const String& p_member);

	// Wrappers.
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void insert(const var& p_key, const var& p_value);
	var& operator[](const var& p_key) const;
	var& operator[](const var& p_key);
	var& operator[](const char* p_key) const;
	var& operator[](const char* p_key);
	stdmap<var, var>::iterator begin() const;
	stdmap<var, var>::iterator end() const;
	stdmap<var, var>::iterator find(const var& p_key) const;
	void clear() { _data->clear(); }
	bool has(const var& p_key) const;
	// TODO:

	// Operators.
	operator bool() const { return empty(); }
	String to_string() const; // operator String() const;
	bool operator ==(const Map& p_other) const;
	Map& operator=(const Map& p_other);

private:
	friend class var;
	ptr<stdmap<var, var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Map& p_dict);
};

}

#endif // MAP_H

#ifndef OBJECT_H
#define OBJECT_H

//include "varhcore.h"

#define REGISTER_CLASS(m_class, m_inherits)                                                          \
public:                                                                                              \
	static  const char* get_class_name_s() { return STR(m_class); }                                  \
	virtual const char* get_class_name() const override { return get_class_name_s(); }               \
	static  const char* get_parent_class_name_s() { return STR(m_inherits); }                        \
	virtual const char* get_parent_class_name() const override { return get_parent_class_name_s(); } \
	static void _bind_data()

namespace varh {

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

	static var call_method(ptr<Object> p_self, const String& p_name, stdvec<var>& p_args);  // instance.p_name(args)
	virtual var __call(stdvec<var>& p_vars);                                                // instance(args)

	static var get_member(ptr<Object> p_self, const String& p_name);
	static void set_member(ptr<Object> p_self, const String& p_name, var& p_value);

	virtual var __get_mapped(const var& p_key) const;
	virtual void __set_mapped(const var& p_key, const var& p_val);

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
	virtual ptr<Object> copy(bool p_deep)         const { throw VarError(VarError::NOT_IMPLEMENTED); }
	static  const char* get_class_name_s()              { return "Object"; }
	virtual const char* get_class_name()          const { return get_class_name_s(); }
	static  const char* get_parent_class_name_s()       { return ""; }
	virtual const char* get_parent_class_name()   const { return get_parent_class_name_s(); }

	static void _bind_data();

private:
	friend class var;
};

}


#endif //OBJECT_H

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

#define DATA_MEM_SIZE 4 * sizeof(real_t)

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

		// math types
		VECT2F,
		VECT2I,
		VECT3F,
		VECT3I,

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
	var(int64_t p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const String& p_string);
	var(const Vect2f& p_vect2f);
	var(const Vect2i& p_vect2i);
	var(const Vect3f& p_vect3f);
	var(const Vect3i& p_vect3i);
	var(const Array& p_array);
	var(const Map& p_map);
	~var();
	
	template <typename T=Object>
	var(const ptr<T>& p_ptr) {
		type = OBJECT;
		_data._obj = p_ptr;
	}

	// Methods.
	inline Type get_type() const { return type; }
	static String get_type_name_s(var::Type p_type);
	String get_type_name() const;
	//const char* get_parent_class_name() const;
	void clear();
	var copy(bool p_deep = false) const;


	// Operators.
	operator bool() const;
	operator int64_t() const;
	operator int() const { return (int)operator int64_t(); }
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator String() const;   // int.operator String() is invalid casting.
	String to_string() const;  // int.to_string() is valid.
	// this treated as: built-in C++ operator[](const char *, int), conflict with operator[](size_t)
	// operator const char* () const;
	operator Vect2f() const;
	operator Vect2i() const;
	operator Vect3f() const;
	operator Vect3i() const;
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

	template <typename... Targs>
	var __call(Targs... p_args) {
		stdvec<var> args;
		__build_args_recur(args, p_args...);
		return __call_internal(args);
	}
	template <typename... Targs>
	var call_method(const String& p_method, Targs... p_args) {
		stdvec<var> args;
		__build_args_recur(args, p_args...);
		return call_method_internal(p_method, args);
	}

	var get_member(const String& p_name);
	void set_member(const String& p_name, var& p_value);

private:
	var __call_internal(stdvec<var>& p_args);
	// TODO: call static func in every var classes.
	var call_method_internal(const String& p_method, stdvec<var>& p_args);
	template <typename T, typename... Targs>
	constexpr void __build_args_recur(stdvec<var>& p_args_arr, T p_val, Targs... p_args) {
		p_args_arr.push_back(p_val);
		__build_args_recur(p_args_arr, p_args...);
	}
	void __build_args_recur(stdvec<var>& p_args_arr) { return; }
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

		Map _map;
		Array _arr;
		ptr<Object> _obj;

		union {
			String _string;

			bool _bool;
			int64_t _int;
			double _float;
			uint8_t _mem[DATA_MEM_SIZE];
		};
	};

	// Methods.
	void copy_data(const var& p_other);
	void clear_data();

	// Members.
	static var tmp;
	Type type;
	VarData _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);
};

// ******** MEMBER INFO IMPLEMENTATIONS ******************* //

class MethodInfo : public MemberInfo {
private:
	String name;
	bool _is_static = false;
	int arg_count = 0; // -1 means static
	stdvec<String> arg_names;
	stdvec<var> default_args;
	stdvec<var::Type> arg_types;
	var::Type return_type;

public:
	virtual Type get_type() const override { return Type::METHOD; }
	virtual const String& get_name() const override { return name; }

	// complete constructor.
	MethodInfo(
		String p_name,
		stdvec<String> p_arg_names,
		stdvec<var::Type> p_arg_types = stdvec<var::Type>(),
		var::Type p_return_type = var::_NULL,
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
		var::Type p_return_type = var::_NULL,
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
	const stdvec<var::Type>& get_arg_types() const { return arg_types; }
	var::Type get_return_type() const { return return_type; }
};

class PropertyInfo : public MemberInfo {
private:
	String name;
	bool _is_static = false;
	bool _is_const = false;
	var value;

public:
	virtual Type get_type() const override { return Type::PROPERTY; }
	virtual const String& get_name() const override { return name; }

	bool is_static() const { return _is_static; }
	bool is_const() const { return _is_const; }
	const var& get_value() const { return value; }         // value for constants.
	const var& get_default_value() const { return value; } // defalut_value for variables.
};

class EnumInfo : public MemberInfo {
private:
	String name;
	stdvec<std::pair<String, uint64_t>> values;

public:
	virtual Type get_type() const override { return Type::ENUM; }
	virtual const String& get_name() const override { return name; }

	const stdvec<std::pair<String, uint64_t>>& get_values() const { return values; }
};

class EnumValueInfo : public MemberInfo {
private:
	String name;
	uint64_t value;

public:
	virtual Type get_type() const override { return Type::ENUM_VALUE; }
	virtual const String& get_name() const override { return name; }

	uint64_t get_value() const { return value; }
};

}

// undefine all var.h macros defined in varcore.h
// this makes the user(carbon) independent of'em
#if defined(UNDEF_VAR_DEFINES)
#if !defined(VAR_H_HEADER_ONLY)

#undef func
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
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
		if (get_member(p_method)->get_type() != MemberInfo::METHOD)																						\
			THROW_ERROR(VarError::INVALID_GET_NAME, String::format("member \"%s\" is not callable.", p_method.c_str()));								\
		const MethodInfo* mp = (MethodInfo*)get_member(p_method);																						\
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
			if (mp->get_arg_types()[j] == var::VAR) continue; /* can't be _NULL. */																		\
			if (p_args.size() == j) break; /* rest are default args. */																					\
			if (mp->get_arg_types()[j] != p_args[j].get_type())																							\
				THROW_ERROR(VarError::INVALID_ARGUMENT, String::format(																					\
					"expected type \"%s\" at argument %i.", var::get_type_name_s(mp->get_arg_types()[j]), j));											\
		}																																				\
	} else {																																			\
		THROW_ERROR(VarError::INVALID_GET_NAME, ""); /* TODO: more clear message. */																	\
	}																																					\
} while (false)

#define MEMBER_INFO_IMPLEMENTATION(m_type)                                                                                                              \
bool m_type::has_member(const String& p_member) {																										\
	return get_member_info().find(p_member) != get_member_info().end();																					\
}																																						\
const MemberInfo* m_type::get_member(const String& p_member) {																							\
	if (!has_member(p_member)) THROW_ERROR(VarError::INVALID_KEY, String::format("member \"%s\" not exists on base " #m_type , p_member.c_str()));		\
	const stdmap<String, const MemberInfo*>& member_info = get_member_info();																			\
	return member_info.at(p_member);																													\
}																																						\
const stdmap<String, const MemberInfo*>& m_type::get_member_info()

// String -----------------------------------------------

MEMBER_INFO_IMPLEMENTATION(String) {
	static const stdmap<String, const MemberInfo*> member_info = {
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
		{ "size",      new MethodInfo("size", var::INT),								     },
		{ "empty",     new MethodInfo("empty", var::BOOL),							         },
		{ "push_back", new MethodInfo("push_back", {"element"}, {var::VAR}, var::_NULL),     },
		{ "pop_back",  new MethodInfo("pop_back", var::_NULL),							     },
		{ "append",    new MethodInfo("append", {"element"}, {var::VAR}, var::ARRAY),	     },
		{ "pop",       new MethodInfo("pop", var::VAR),									     },
		{ "clear",     new MethodInfo("clear", var::_NULL),								     },
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
		"var"
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


#endif // VAR_IMPLEMENTATION