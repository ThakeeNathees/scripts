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
#include "compile_time_crc32.inc"

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

		CLASS, // for FileNodes
	};

	virtual Type get_type() const = 0;
	virtual const String& get_name() const = 0;
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