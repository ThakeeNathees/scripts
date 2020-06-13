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

#include "varhcore.h"

namespace varh {

class Array
{
private:
	friend class var;
	Ptr<std::vector<var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr);
public:
	/* constructors */
	Array() {
		_data = newptr(std::vector<var>);
	}
	Array(const Ptr<std::vector<var>>& p_data) {
		_data = p_data;
	}
	Array(const Array& p_copy) {
		_data = p_copy._data;
	}

	// TODO: try implementing with packed variadic template 
	//template<typename... T> Array(const T&... p_vars) {
	//	_data = newptr(std::vector<var>);
	//	var arr[] = { (p_vars)... };
	//	for (size_t i = 0; i < sizeof(arr); i++) {
	//		push_back(arr[i]);
	//	}
	//}

	std::vector<var>* get_data() {
		return _data.operator->();
	}
	std::vector<var>* get_data() const {
		return _data.operator->();
	}

	Array copy(bool p_deep = true) const;

	/* wrappers */
	// TODO: throw all errors with VarError
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void push_back(const var& p_var) { _data->push_back(p_var); }
	void append(const var& p_var) { push_back(p_var); }
	void pop_back() { _data->pop_back(); }
	var& pop() { var& ret = this->operator[](size() - 1); pop_back(); return ret; } 
	var& operator[](size_t p_pos) const { return _data->operator[](p_pos); }
	var& operator[](size_t p_pos) { return _data->operator[](p_pos); }
	std::vector<var>::const_iterator begin() const { return (*_data).begin(); }
	std::vector<var>::const_iterator end() const { return (*_data).end(); }
	void clear() { (*_data).clear(); }
	var& at(size_t p_pos) { return (*_data).at(p_pos); }
	var& back() { return (*_data).back(); }
	var& front() { return (*_data).front(); }
	// TODO: 

	/* cast operators */
	operator bool() const { return empty(); }
	operator String() const;
	bool operator ==(const Array& p_other) const;
	Array& operator=(const Array& p_other);
	Array operator+(const Array& p_other) const;
	Array& operator+=(const Array& p_other);
};

}

#endif // ARRAY_H