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

class String //: public std::string
{
private:
	friend class var;
	std::string _data;

public:
	String() {}
	String(const std::string& p_copy) { _data = p_copy; }
	String(const char* p_copy) { _data = p_copy; }
	String(const String& p_copy) { _data = p_copy._data; }
	String(const char& p_char) { _data = p_char; }

	static String format(const char* p_format, ...);

	~String() { }

	int to_int() const { return std::stoi(_data); }
	double to_double() const { return std::stod(_data); }


	String& operator=(const String& p_other) {
		_data = p_other._data;
		return *this;
	}
	String& operator+=(const String& p_other) {
		_data += p_other._data;
		return *this;
	}
	char operator[](size_t p_index) const {
		// TODO: VAR_ERR
		return _data[p_index];
	}
	char& operator[](size_t p_index) {
		// TODO: VAR_ERR
		return _data[p_index];
	}

	String operator+(const char* p_cstr) const { return _data + p_cstr; }
	String operator+(const String& p_other) const { return _data + p_other._data; }
	bool operator==(const String & p_other) const { return _data == p_other._data; }
	bool operator!=(const String & p_other) const { return _data != p_other._data; }
	bool operator<(const String& p_other) const { return _data < p_other._data; }
	operator std::string() const { return _data; }

	String& operator+=(const char& p_c) { _data += p_c; return *this; }

	// wrappers
	size_t size() const { return _data.size(); }
	const char* c_str() const { return _data.c_str(); }
	String& append(const String& p_other) { _data.append(p_other); return *this; }
};

// global operations // TODO: implement more
bool operator==(const char* p_cstr, const String& p_str);
bool operator!=(const char* p_cstr, const String& p_str);


}

#endif // STRING_H