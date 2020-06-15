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

#ifndef  MAP_H
#define  MAP_H

#include "varhcore.h"

namespace varh {

class var;
class String;

class Map {
public:
	// Mehtods.
	Map() {
		_data = std::make_shared<std::map<var, var>>();
	}
	Map(const ptr<std::map<var, var>>& p_data) {
		_data = p_data;
	}
	Map(const Map& p_copy) {
		_data = p_copy._data;
	}

	std::map<var, var>* get_data() {
		return _data.operator->();
	}
	std::map<var, var>* get_data() const {
		return _data.operator->();
	}

	Map copy(bool p_deep = true) const;

	// Wrappers.
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	var& operator[](const var& p_key) const;
	var& operator[](const var& p_key);
	std::map<var, var>::iterator begin() const;
	std::map<var, var>::iterator end() const;
	std::map<var, var>::iterator find(const var& p_key) const;
	void clear() { _data->clear(); }
	bool has(const var& p_key) const;
	// TODO:

	// Operators.
	operator bool() const { return empty(); }
	operator String() const;
	bool operator ==(const Map& p_other) const;
	Map& operator=(const Map& p_other);

private:
	friend class var;
	ptr<std::map<var, var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Map& p_dict);
};

}

#endif // MAP_H