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

#include <memory>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <string>
#include <type_traits>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <map>

#define EVAL(...) __VA_ARGS__
#define ARGCOUNT_I(_,_9,_8,_7,_6,_5,_4,_3,_2,_1,X_,...) X_
#define ARGCOUNT(...) \
   ARGCOUNT_I(__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)

#define STR_CAT2(m_a, m_b) m_a##m_b
#define STR_CAT3(m_a, m_b, m_c) m_a##m_b##m_c
#define STR_CAT4(m_a, m_b, m_c, m_d) m_a##m_b##m_c##m_d

#define STR(m_mac) #m_mac
#define STRINGIFY(m_mac) STR(m_mac)

#define M_PLACE_HOLDER

#ifdef _DEBUG
#define DBG                                             \
do {                                                    \
	printf("DBG: (%s:%i)\n", __FILE__, __LINE__);       \
} while (false)
#else
#define DBG
#endif

#define newref_t1(T1, ...) std::make_shared<T1>(__VA_ARGS__);
#define newref_t2(T1, T2, ...) std::make_shared<T1, T2>(__VA_ARGS__);
template<typename T>
using Ref = std::shared_ptr<T>;

#if defined(DOUBLE_AS_REAL)
typedef double real_t;
#else
typedef float real_t;
#endif

typedef void(*VarErrCallback)(const char* p_msg, const char* p_func, const char* p_file, int p_line);
void var_set_err_callback(const VarErrCallback p_callback);
VarErrCallback var_get_err_callback();

#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

#define VAR_ERR(m_msg) \
	var_get_err_callback()(m_msg, __FUNCTION__, __FILE__, __LINE__)

#define VAR_DEBUG // TODO
#if defined(VAR_DEBUG)
#define VAR_ASSERT(m_cond, m_msg)  \
	if (!(m_cond))                 \
		VAR_ERR(m_msg)
#else
#define VAR_ASSERT
#endif


#endif // VARHCORE_H