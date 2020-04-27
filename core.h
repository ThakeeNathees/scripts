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

#ifndef CORE_H
#define CORE_H

#include <memory>
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <sstream>

#define STR_CAT2(m_a, m_b) m_a##m_b
#define STR_CAT3(m_a, m_b, m_c) m_a##m_b##m_c
#define STR_CAT4(m_a, m_b, m_c, m_d) m_a##m_b##m_c##m_d

#define newref(T, ...) std::make_shared<T>(__VA_ARGS__);
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


#endif // CORE_H