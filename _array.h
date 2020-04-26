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

#ifndef  ARRAY_H
#define  ARRAY_H

#include "core.h"

class Array
{
private:
	friend class var;
	Ref<std::vector<var>> _data_arr;
public:
	/* constructors */
	Array() {
		//type = ARRAY;
		_data_arr = newref(std::vector<var>);
	}
	Array(const Ref<std::vector<var>>& p_data) {
		//type = ARRAY;
		_data_arr = p_data;
	}
	Array(const Array& p_copy) {
		//type = ARRAY;
		_data_arr = p_copy._data_arr;
	}

	std::vector<var>* get_data() {
		return _data_arr.operator->();
	}

	/* wrappers */
	bool empty() const { return _data_arr->empty(); }
	void push_back(const var& p_var) { _data_arr->push_back(p_var); }
	var& operator[](size_t p_pos) { return _data_arr->operator[](p_pos); }

	/* cast operators */
	operator bool() { return empty(); }
};

#endif // ARRAY_H