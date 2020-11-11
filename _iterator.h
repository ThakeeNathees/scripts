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

#ifndef _ITERATOR_H
#define _ITERATOR_H

// DO NOT INCLUDE THIS AS IT WON'T WORK SINCE IT HAS DEPENDANCY IN _var.h
// INCLUDE _var.h INSETEAD. IT'S DESIGNED LIKE THIS TO GENERATE SINGLE HEADER.

namespace varh {

class _Iterator_String : public Object {
	REGISTER_CLASS(_Iterator_String, Object) {
		BIND_METHOD("__iter_has_next", &_Iterator_String::__iter_has_next);
		BIND_METHOD("__iter_next", &_Iterator_String::__iter_next);
	}
	size_t _it = 0;
	const String* _str_data = nullptr;

public:
	_Iterator_String() {}
	_Iterator_String(const String* p_str) : _str_data(p_str) {}

	bool __iter_has_next() override {
		return _it != _str_data->size();
	}

	var __iter_next() override {
		return String(_str_data->operator[](_it++));
	}
};

class _Iterator_Array : public Object {
	REGISTER_CLASS(_Iterator_Array, Object) {
		BIND_METHOD("__iter_has_next", &_Iterator_Array::__iter_has_next);
		BIND_METHOD("__iter_next", &_Iterator_Array::__iter_next);
	}
	stdvec<var>::iterator _it;
	stdvec<var>* _array_data = nullptr;
public:
	_Iterator_Array() {}
	_Iterator_Array(const Array* p_array) {
		_array_data = p_array->get_data();
		_it = _array_data->begin();
	}

	virtual bool __iter_has_next() override {
		return _it != _array_data->end();
	}
	virtual var __iter_next() override {
		return *(_it++);
	}

};

class _Map_KeyValue_Pair : public Object {
	REGISTER_CLASS(_Map_KeyValue_Pair, Object) {}

public:
	_Map_KeyValue_Pair() {}
	_Map_KeyValue_Pair(var* p_key, var* p_value) : key(p_key), value(p_value) {}

	var __get_member(const String& p_name) {
		if (p_name == "key") {
			return *key;
		} else if (p_name == "value") {
			return *value;
		} else {
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no value named \"%s\".", get_class_name(), p_name.c_str()));
		}
	}
	void __set_member(const String& p_name, var& p_value) override {
		if (p_name == "key") {
			*key = p_value;
		} else if (p_name == "value") {
			*value = p_value;
		} else {
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no value named \"%s\".", get_class_name(), p_name.c_str()));
		}
	}

	var *key, *value;
};

class _Iterator_Map : public Object {
	REGISTER_CLASS(_Iterator_Map, Object) {
		BIND_METHOD("__iter_has_next", &_Iterator_Map::__iter_has_next);
		BIND_METHOD("__iter_next", &_Iterator_Map::__iter_next);
	}
	Map::_map_internal_t::iterator _it;
	Map::_map_internal_t* _map_data = nullptr;

public:

	_Iterator_Map() {}
	_Iterator_Map(const Map* p_map) {
		_map_data = p_map->get_data();
		_it = _map_data->begin();
	}

	virtual bool __iter_has_next() override {
		return _it != _map_data->end();
	}

	virtual var __iter_next() override {
		ptr<_Map_KeyValue_Pair>& ret = newptr<_Map_KeyValue_Pair>(&_it->second.key, &(_it)->second.value);
		_it++;
		return ret;
	}
};

}

#endif // _ITERATOR_H