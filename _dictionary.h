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

#ifndef  DICTIONARY_H
#define  DICTIONARY_H

#include "core.h"

class var;

using dict_iterator = std::_Tree<std::_Tmap_traits<var, var, std::less<var>, std::allocator<std::pair<const var, var>>, false>>::iterator;

class Dictionary
{
private:
	friend class var;
	Ref<std::map<var, var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Dictionary& p_dict);
public:
	/* constructors */
	Dictionary() {
		_data = newref_t2(std::map<var, var>);
	}
	Dictionary(const Ref<std::map<var, var>>& p_data) {
		_data = p_data;
	}
	Dictionary(const Dictionary& p_copy) {
		_data = p_copy._data;
	}

	std::map<var, var>* get_data() {
		return _data.operator->();
	}
	std::map<var, var>* get_data() const {
		return _data.operator->();
	}

	Dictionary copy(bool p_deep = true) const;
	//
	// /* wrappers */
	size_t size() const { return _data->size(); }
	// bool empty() const { return _data->empty(); }
	//void push_back(const var& p_var) { _data->push_back(p_var); }
	//var& operator[](const var& p_key) const { return (*_data)[p_key]; }
	//var& operator[](const var& p_key) { return (*_data)[p_key]; }
	//dict_iterator begin() const { return (*_data).begin(); }
	//dict_iterator end() const { return (*_data).end(); }
	//void clear() { (*_data).clear(); }
	//bool empty() const { return (*_data).empty(); }
	// // TODO:

	// /* cast operators */
	//operator bool() const { return empty(); }
	//operator std::string() const;
	//operator const char* () const {
	//	return operator std::string().c_str();
	//}
	//bool operator ==(const Dictionary& p_other) const;
	// Array operator+(const Array& p_other) const;
	// Array& operator+=(const Array& p_other);
};

#endif // DICTIONARY_H