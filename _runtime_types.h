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

#ifndef _RUNTIME_TYPES_H
#define _RUNTIME_TYPES_H

// DO NOT INCLUDE THIS AS IT WON'T WORK SINCE IT HAS DEPENDANCY IN _var.h
// INCLUDE _var.h INSETEAD. IT'S DESIGNED LIKE THIS TO GENERATE SINGLE HEADER.

namespace varh {

class NativeClasses;

// A runtime handle for enum types and bytecode for enums.
class _EnumBytes : public Object {
	REGISTER_CLASS(_EnumBytes, Object) { }

public:

	_EnumBytes() {}
	_EnumBytes(const String& p_name) : _name(p_name) {}
	_EnumBytes(const String& p_name, const stdvec<std::pair<String, int64_t>>& p_values) {
		_name = p_name;
		for (const std::pair<String, int64_t>& p : p_values) {
			_values[p.first] = p.second;
		}
	}

	var __get_member(const String& p_name) override {
		stdmap<String, int64_t>::iterator it = _values.find(p_name);
		if (it != _values.end()) return it->second;
		else THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("enum %s has no value named \"%s\".", _name.c_str(), p_name.c_str()));
	}
	void __set_member(const String& p_name, var& p_value) override {
		stdmap<String, int64_t>::iterator it = _values.find(p_name);
		if (it != _values.end()) THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("cannot assign a value to enum value."));
		else THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("enum %s has no member named \"%s\".", _name.c_str(), p_name.c_str()));
	}

public: // like a struct.
	String _name;
	stdmap<String, int64_t> _values;
};


}

#endif // _RUNTIME_TYPES_H
