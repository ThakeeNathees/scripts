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