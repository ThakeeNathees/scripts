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

#ifndef _VAR_H
#define _VAR_H

#include "_array.h"
#include "_vector.h"

#define DATA_PTR(T) reinterpret_cast<const T *>(_data._mem)
// TODO: var fn = &func; fn(); operator(){}

class var
{
public:
	enum Type
	{
		_NULL,
		BOOL,
		INT,
		FLOAT,
		STD_STRING,

		// math types
		VECT2F,
		VECT2I,
		VECT3F,
		VECT3I,

		// misc types
		ARRAY,
		OBJ_PTR,

		TYPE_MAX,
	};

protected:
	Type type = _NULL;

private:
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);

	struct _DataObj
	{
		size_t hash_code;
		const char* name;
		const void* _ptr;
		_DataObj(const size_t hash_code, const char* name, const void* _ptr)
			: hash_code(hash_code), name(name), _ptr(_ptr)
		{}
		_DataObj()
			: hash_code(-1), name("???"), _ptr(nullptr)
		{}
	};

	std::string _data_std_string;
	Array _data_arr;
	_DataObj _data_obj;
	union {
		bool _bool;
		int _int;
		double _float;
		uint8_t _mem[4 * sizeof(real_t)];
	} _data;

public:
	/* public api */
	inline Type get_type() const { return type; }
	void clear();

	/* constructors */
	var();
	var(const var& p_copy) = default;
	var(bool p_bool);
	var(int p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const std::string& p_std_string);
	var(const Vect2f& p_vect2f);
	var(const Vect2i& p_vect2i);
	var(const Vect3f& p_vect3f);
	var(const Vect3i& p_vect3i);
	var(const Array& p_array);
	template<typename T> var(const T& p_obj) {
		type = OBJ_PTR;
		const void * _ptr = (const void *)&p_obj;
		_data_obj = _DataObj(typeid(T).hash_code(), typeid(T).name(), _ptr);
	}
	template<typename T> var(const T* p_obj) {
		type = OBJ_PTR;
		const void* _ptr = (const void*)p_obj;
		_data_obj = _DataObj(typeid(T).hash_code(), typeid(T).name(), _ptr);
	}


	/* casting */
	operator bool() const;
	operator int() const;
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator std::string() const;
	operator Vect2f() const;
	operator Vect2i() const;
	operator Vect3f() const;
	operator Vect3i() const;
	operator Array() const;

	template<typename T>
	T* as() const {
		VAR_ASSERT(type == OBJ_PTR, "invalid casting");
		return (T*)_data_obj._ptr;
	}
	template<typename T>
	bool is() const {
		switch (type) {
			case _NULL:
			case BOOL:
			case INT:
			case FLOAT:
				return false;
			case STD_STRING: return typeid(_data_std_string) == typeid(T);
			case VECT2F: return typeid(*DATA_PTR(Vect2f)) == typeid(T);
			case VECT2I: return typeid(*DATA_PTR(Vect2i)) == typeid(T);
			case VECT3F: return typeid(*DATA_PTR(Vect3f)) == typeid(T);
			case VECT3I: return typeid(*DATA_PTR(Vect3i)) == typeid(T);
			case ARRAY: return typeid(_data_arr) == typeid(T);
			case OBJ_PTR: return _data_obj.hash_code == typeid(T).hash_code();
				break;
		}
		VAR_ERR("invalid var type");
		return false;
	}

	///* operator overloading */
	//	/* comparison */
	//bool operator==(const var& p_other) const;
	//bool operator!=(const var& p_other) const;
	//bool operator<=(const var& p_other) const;
	//bool operator>=(const var& p_other) const;
	//bool operator<(const var& p_other) const;
	//bool operator>(const var& p_other) const;

	//	/* unaray */
	var operator++();
	var operator++(int);
	var operator--();
	var operator--(int);
	bool operator !() const { return !operator bool(); }

	//	/* binary */
	//var operator+(const var& p_other) const;
	//var operator-(const var& p_other) const;
	//var operator*(const var& p_other) const;
	//var operator/(const var& p_other) const;
	//var operator%(const var& p_other) const;

	//	/* assignments */
	var& operator=(const var& p_other) = default;
	//var& operator+=(const var& p_other);
	//var& operator-=(const var& p_other);
	//var& operator*=(const var& p_other);
	//var& operator/=(const var& p_other);
	//var& operator%=(const var& p_other);

	~var();

};

#endif // _VAR_H