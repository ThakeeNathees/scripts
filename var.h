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


#ifndef  ARRAY_H
#define  ARRAY_H


#ifndef CORE_H
#define CORE_H

#include <memory>
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#define STR_CAT2(m_a, m_b) m_a##m_b
#define STR_CAT3(m_a, m_b, m_c) m_a##m_b##m_c
#define STR_CAT4(m_a, m_b, m_c, m_d) m_a##m_b##m_c##m_d

#define newref(T, ...) std::make_shared<T>(__VA_ARGS__);
template<typename T>
using Ref = std::shared_ptr<T>;

#if defined(DOUBLE_AS_REAL)
typedef double real_t;
#else
typedef float real_t;
#endif

typedef void(*VarErrCallback)(const char* p_msg, const char* p_func, const char* p_file, int p_line);
void var_set_err_callback(const VarErrCallback p_callback);
VarErrCallback var_get_err_callback();

#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

#define VAR_ERR(m_msg) \
	var_get_err_callback()(m_msg, __FUNCTION__, __FILE__, __LINE__)

#define VAR_DEBUG // TODO
#if defined(VAR_DEBUG)
#define VAR_ASSERT(m_cond, m_msg)  \
	if (!(m_cond))                 \
		VAR_ERR(m_msg)
#else
#define VAR_ASSERT
#endif


#endif // CORE_H

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

#ifndef VECTOR_H
#define VECTOR_H

//include "core.h"

#define VECT2_DEFAULT_CMP_OP_OVERLOAD(m_op)            \
bool operator m_op (const Vect2<T>& p_other) const {   \
	return get_length() m_op p_other.get_length();     \
}
#define VECT3_DEFAULT_CMP_OP_OVERLOAD(m_op)            \
bool operator m_op (const Vect3<T>& p_other) const {   \
	return get_length() m_op p_other.get_length();     \
}

template<typename T>
struct Vect2
{
	union { T x, width; };
	union { T y, height; };

	/* constructors */
	Vect2(T p_x = 0, T p_y = 0) : x(p_x), y(p_y) {}
	Vect2(const T* p_arr) : x(p_arr[0]), y(p_arr[1]) {}
	template<typename T2>
	Vect2(const Vect2<T2>& p_copy) : x((T)p_copy.x), y((T)p_copy.y) {}

	float get_length() const {
		return sqrtf(x * x + y * y);
	}
	float get_angle() const {
		if (x == 0) return (float)M_PI_2;
		return atanf(y / x);
	}

	/* math operators */
	Vect2<T> operator+(const Vect2<T>& p_other) const {
		return Vect2<T>(x + p_other.x, y + p_other.y);
	}
	Vect2<T> operator-(const Vect2<T>& p_other) const {
		return Vect2<T>(x - p_other.x, y - p_other.y);
	}
	Vect2<T> operator*(const Vect2<T>& p_other) const {
		return Vect2<T>(x * p_other.x, y * p_other.y);
	}
	Vect2<T> operator/(const Vect2<T>& p_other) const {
		return Vect2<T>(x / p_other.x, y / p_other.y);
	}

	VECT2_DEFAULT_CMP_OP_OVERLOAD(<);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(<=);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>=);
	bool operator==(const Vect2<T>& p_other) const {
		return x == p_other.x && y == p_other.y;
	}
	bool operator!=(const Vect2<T>& p_other) const {
		return !operator==(p_other);
	}

	Vect2<T>& operator+=(const Vect2<T>& p_other) {
		x += p_other.x; y += p_other.y;
		return *this;
	}
	Vect2<T>& operator-=(const Vect2<T>& p_other) {
		x -= p_other.x; y -= p_other.y;
		return *this;
	}
	Vect2<T>& operator*=(const Vect2<T>& p_other) {
		x *= p_other.x; y *= p_other.y;
		return *this;
	}
	Vect2<T>& operator/=(const Vect2<T>& p_other) {
		x /= p_other.x; y /= p_other.y;
		return *this;
	}

	/* cast operators */
	operator bool() const { return x == 0 && y == 0; }
	operator std::string() const { // FIXME
		return std::string("Vect2(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(")");
	}

};


template<typename T>
struct Vect3
{
	union { T x, width; };
	union { T y, height; };
	union { T z, depth; };

	/* constructors */
	Vect3(T p_x = 0, T p_y = 0, T p_z = 0) : x(p_x), y(p_y), z(p_z) {}
	Vect3(const T* p_arr) : x(p_arr[0]), y(p_arr[1]), z(p_arr[2]) {}
	template<typename T2>
	Vect3(const Vect3<T2>& p_copy) : x((T)p_copy.x), y((T)p_copy.y), z((T)p_copy.z) {}

	float get_length() const {
		return sqrtf(x * x + y * y + z * z);
	}

	Vect3<T> operator+(const Vect3<T>& p_other) const {
		return Vect3<T>(x + p_other.x, y + p_other.y, z + p_other.z);
	}
	Vect3<T> operator-(const Vect3<T>& p_other) const {
		return Vect3<T>(x - p_other.x, y - p_other.y, z - p_other.z);
	}
	Vect3<T> operator*(const Vect3<T>& p_other) const {
		return Vect3<T>(x * p_other.x, y * p_other.y, z * p_other.z);
	}
	Vect3<T> operator/(const Vect3<T>& p_other) const {
		return Vect3<T>(x / p_other.x, y / p_other.y, z / p_other.z);
	}

	VECT2_DEFAULT_CMP_OP_OVERLOAD(<);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(<=);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>=);
	bool operator==(const Vect3<T>& p_other) const {
		return x == p_other.x && y == p_other.y && z == p_other.z;
	}
	bool operator!=(const Vect3<T>& p_other) const {
		return !operator==(p_other);
	}

	Vect3<T>& operator+=(const Vect3<T>& p_other) {
		x += p_other.x; y += p_other.y; z += p_other.z;
		return *this;
	}
	Vect3<T>& operator-=(const Vect3<T>& p_other) {
		x -= p_other.x; y -= p_other.y; z -= p_other.z;
		return *this;
	}
	Vect3<T>& operator*=(const Vect3<T>& p_other) {
		x *= p_other.x; y *= p_other.y; z *= p_other.z;
		return *this;
	}
	Vect3<T>& operator/=(const Vect3<T>& p_other) {
		x /= p_other.x; y /= p_other.y; z /= p_other.z;
		return *this;
	}

	/* cast operators */
	operator bool() const { return x == 0 && y == 0 && z == 0; }
	operator std::string() const { // FIXME
		return std::string("Vect3(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(", ")
			.append(std::to_string(z)).append(")");
	}
};


/* typedefs */
typedef Vect2<float> Vect2f;
typedef Vect2<int> Vect2i;
typedef Vect3<float> Vect3f;
typedef Vect3<int> Vect3i;

#endif //VECTOR_H

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

//--------------- VAR_IMPLEMENTATION -------------------

#if defined(VAR_IMPLEMENTATION)

//include "_var.h"

std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.operator std::string();
	return p_ostream;
}

// FIXME
static void var_err_callback(const char* p_msg, const char* p_func, const char* p_file, int p_line) {
	std::cout << "ERROR: " << p_msg << std::endl
		<< "\tat: " << p_func << "(" << p_file << ":" << p_line <<")" << std::endl;
	DEBUG_BREAK();
}
static VarErrCallback s_var_err_callback = var_err_callback;
void var_set_err_callback(const VarErrCallback p_callback) {
	s_var_err_callback = p_callback;
}
VarErrCallback var_get_err_callback() {
	return s_var_err_callback;
}

/* clear */
void var::clear() {
	type = _NULL;
}

/* constructors */
var::var() {
	data._bool = false;
	type = _NULL;
}

var::var(bool p_bool) {
	type = BOOL;
	data._bool = p_bool;
}

var::var(int p_int) {
	type = INT;
	data._int = p_int;
}

var::var(float p_float) {
	type = FLOAT;
	data._float = p_float;
}

var::var(double p_double) {
	type = FLOAT;
	data._float = p_double;
}

var::var(const char* p_cstring) {
	type = STD_STRING;
	data_std_string = std::string(p_cstring);
}

var::var(const std::string& p_std_string) {
	type = STD_STRING;
	data_std_string = std::string(p_std_string);
}

var::var(const Array& p_array) {
	type = ARRAY;
	_data_arr = p_array._data_arr;
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)             \
var var::operator m_op () {                    \
	switch (type) {                            \
		case INT:  return m_op data._int;      \
		case FLOAT: return m_op data._float;   \
		default: VAR_ERR("invalid casting");   \
	}                                          \
	return var();                              \
}

#define VAR_OP_POST_INCR_DECR(m_op)            \
var var::operator m_op(int) {                  \
	switch (type) {                            \
		case INT: return data._int m_op;       \
		case FLOAT: return data._float m_op;   \
		default: VAR_ERR("invalid casting");   \
	}                                          \
	return var();                              \
}
VAR_OP_PRE_INCR_DECR(++)
VAR_OP_PRE_INCR_DECR(--)
VAR_OP_POST_INCR_DECR(++)
VAR_OP_POST_INCR_DECR(--)


/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL: return false;
		case BOOL: return data._bool;
		case INT: return data._int != 0;
		case FLOAT: return data._float != 0;
		case STD_STRING: return data_std_string.size() != 0;

		case VECT2F: return *DATA_PTR(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR(Vect3f) == Vect3f();
		case ARRAY: return _data_arr.empty();
			break; // TODO:

	}
	VAR_ERR("invalid casting");
	return false;
}

var::operator int() const {
	switch (type) {
		case BOOL: return data._bool;
		case INT: return data._int;
		case FLOAT: return (int)data._float;
		case STD_STRING: return  std::stoi(data_std_string);
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator double() const {
	switch (type) {
		case BOOL: return data._bool;
		case INT: return data._int;
		case FLOAT: return data._float;
		case STD_STRING: return  std::stod(data_std_string);
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator std::string() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (data._bool) ? "true" : "false";
		case INT: return std::to_string(data._int);
		case FLOAT: return std::to_string(data._float);
		case STD_STRING: return data_std_string;
		case VECT2F: return *DATA_PTR(Vect2f);
		case VECT2I: return *DATA_PTR(Vect2i);
		case VECT3F: return *DATA_PTR(Vect3f);
		case VECT3I: return *DATA_PTR(Vect3i);
		case ARRAY:
			break;
	}
	VAR_ERR("invalid casting");
	return "TODO";
}


#define D_VEC(m_vect, m_dim, m_t) STR_CAT3(m_vect, m_dim, m_t)
#define VAR_VECT_CAST(m_dim, m_t)                                                 \
var::operator D_VEC(Vect, m_dim, m_t)() const {                                   \
	switch (type) {                                                               \
		case D_VEC(VECT, m_dim, F): return *DATA_PTR(D_VEC(Vect, m_dim, f));      \
		case D_VEC(VECT, m_dim, I): return *DATA_PTR(D_VEC(Vect, m_dim, i));      \
		default: VAR_ERR("invalid casting");                                      \
	}                                                                             \
	return D_VEC(Vect, m_dim, m_t)();                                             \
}
VAR_VECT_CAST(2, f)
VAR_VECT_CAST(2, i)
VAR_VECT_CAST(3, f)
VAR_VECT_CAST(3, i)

var::operator Array() const {
	switch (type) {
		case ARRAY: return Array(_data_arr);
		default: VAR_ERR("invalid casting");
	}
	return Array();
}


#endif // VAR_IMPLEMENTATION