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

#define DATA_PTR(T) reinterpret_cast<const T *>(data._mem)
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
		// TODO: obj ref

		TYPE_MAX,
	};

protected:
	Type type = _NULL;

private:
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);


	std::string data_std_string;
	Array _data_arr;
	union {
		bool _bool;
		int _int;
		double _float;
		// TODO: reference to object
		uint8_t _mem[4 * sizeof(real_t)];
	} data;

public:
	/* public api */
	inline Type get_type() const { return type; }
	void clear();

	/* constructors */
	var();
	var(bool p_bool);
	var(int p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const std::string& p_std_string);
	var(const Array& p_array);
	var(const var& p_copy) = default;


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