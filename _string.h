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

namespace varh {

class var;

class String : public std::string
{
private:
	friend class var;

public:
	String() {}
	String(const std::string& p_copy) : std::string(p_copy) {}
	String(const char* p_copy) : std::string(p_copy) {}

	~String() { }

	int stoi() const { return std::stoi(*this); }
	double stod() const { return std::stod(*this); }

	String operator+(const String& p_other) const {
		return std::operator+(*this, p_other);
	}

	String& operator=(const String& p_other) {
		std::string::operator=(p_other);
		return *this;
	}

	String& operator+=(const String& p_other) {
		std::string::operator+=(p_other);
		return *this;
	}

	char& operator[](size_t p_index) {
		// TODO: VAR_ERR
		return std::string::operator[](p_index);
	}
};

}

#endif // STRING_H