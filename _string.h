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

#ifndef STRING_H
#define STRING_H

#include "varhcore.h"

class var;

class String //: public std::string
{
private:
	friend class var;
	std::string _data;

public:
	String() {}
	String(const std::string& p_copy) : _data(p_copy) {}
	String(const char* p_copy) : _data(p_copy) {}

	int stoi() const { return std::stoi(_data); }
	double stod() const { return std::stod(_data); }

	/* operators */
#define STR_CMP_OP(m_op)                               \
	bool operator m_op(const String& p_other) const {  \
		return _data m_op p_other._data;               \
	}
	STR_CMP_OP(==)
	STR_CMP_OP(<)
	STR_CMP_OP(<=)
	STR_CMP_OP(>)
	STR_CMP_OP(>=)
#undef STR_CMP_OP

	String operator+(const String& p_other) const {
		return _data + p_other._data;
	}

	String& operator+=(const String& p_other) {
		_data += p_other._data;
		return *this;
	}

	/* wrappers */
	// TODO: implement all wrappers and catch errors via VAR_ERR
	size_t size() const { return _data.size(); }
	String& append(const String& p_other) {
		_data.append(p_other._data);
		return *this;
	}
	const char* c_str() const { return _data.c_str(); }
	char at(size_t p_off) { return _data.at(p_off); }
};


#endif // STRING_H