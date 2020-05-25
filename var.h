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

#define VAR_H_HEADER_ONLY

#ifndef _VAR_H
#define _VAR_H


#ifndef STRING_H
#define STRING_H


#ifndef VARHCORE_H
#define VARHCORE_H

#include <memory>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <string>
#include <type_traits>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <map>

#define STRCAT2(m_a, m_b) m_a##m_b
#define STRCAT3(m_a, m_b, m_c) m_a##m_b##m_c
#define STRCAT4(m_a, m_b, m_c, m_d) m_a##m_b##m_c##m_d

#define STR(m_mac) #m_mac
#define STRINGIFY(m_mac) STR(m_mac)

#define PLACE_HOLDER_MACRO

#define newref_t1(T1, ...) std::make_shared<T1>(__VA_ARGS__);
#define newref_t2(T1, T2, ...) std::make_shared<T1, T2>(__VA_ARGS__);
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

#if defined(_DEBUG)
#define VAR_ASSERT(m_cond, m_msg)  \
	if (!(m_cond))                 \
		VAR_ERR(m_msg)
#else
#define VAR_ASSERT
#endif

#endif // VARHCORE_H

class var;

class String //: public std::string
{
private:
	friend class var;
	std::string _data;

public:
	String() {}
	String(const std::string& p_copy) : _data(p_copy) {}
	String(const char* p_copy) : _data(p_copy) {}

	int stoi() const { return std::stoi(_data); }
	double stod() const { return std::stod(_data); }

	/* operators */
#define STR_CMP_OP(m_op)                               \
	bool operator m_op(const String& p_other) const {  \
		return _data m_op p_other._data;               \
	}
	STR_CMP_OP(==)
	STR_CMP_OP(<)
	STR_CMP_OP(<=)
	STR_CMP_OP(>)
	STR_CMP_OP(>=)
#undef STR_CMP_OP

	String operator+(const String& p_other) const {
		return _data + p_other._data;
	}

	String& operator+=(const String& p_other) {
		_data += p_other._data;
		return *this;
	}

	/* wrappers */
	// TODO: implement all wrappers and catch errors via VAR_ERR
	size_t size() const { return _data.size(); }
	String& append(const String& p_other) {
		_data.append(p_other._data);
		return *this;
	}
	const char* c_str() const { return _data.c_str(); }
	char at(size_t p_off) { return _data.at(p_off); }
};


#endif // STRING_H

#ifndef  ARRAY_H
#define  ARRAY_H

//include "varhcore.h"

class var;
class String;

class Array
{
private:
	friend class var;
	Ref<std::vector<var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr);
public:
	/* constructors */
	Array() {
		_data = newref_t1(std::vector<var>);
	}
	Array(const Ref<std::vector<var>>& p_data) {
		_data = p_data;
	}
	Array(const Array& p_copy) {
		_data = p_copy._data;
	}

	// TODO: try implementing with packed variadic template 
	//template<typename... T> Array(const T&... p_vars) {
	//	_data = newref_t1(std::vector<var>);
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
	// TODO: make all errors to VAR_ERR
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
	Array operator+(const Array& p_other) const;
	Array& operator+=(const Array& p_other);
};

#endif // ARRAY_H

#ifndef VECTOR_H
#define VECTOR_H

//include "varhcore.h"

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

	real_t get_length() const {
		return (real_t)sqrtf((real_t)(x * x + y * y));
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
	template <typename T2>
	Vect2<T> operator*(T2 p_val) const {
		return Vect2<T>(x * p_val, y * p_val);
	}
	Vect2<T> operator/(const Vect2<T>& p_other) const {
		if (p_other.x == 0 || p_other.y == 0)
			VAR_ERR("zero division error");
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
		if (p_other.x == 0 || p_other.y == 0)
			VAR_ERR("zero division error");
		x /= p_other.x; y /= p_other.y;
		return *this;
	}

	/* cast operators */
	operator bool() const { return x == 0 && y == 0; }
	operator String() const { // FIXME
		return String("Vect2(")
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

	real_t get_length() const {
		return (real_t)sqrtf((real_t)(x * x + y * y + z * z));
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
		if (p_other.x == 0 || p_other.y == 0 || p_other.z == 0)
			VAR_ERR("zero division error");
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
#define M_OP_TEMPLATE(T2)                    \
	Vect3<T>& operator*=(T2 p_val) const {   \
		x *= p_val; y *= p_val; z *= p_val;  \
		return *this;                        \
	}
	M_OP_TEMPLATE(float)
	M_OP_TEMPLATE(int)
#undef M_OP_TEMPLATE

	template<typename T2>
	Vect3<T>& operator/=(const Vect3<T2>& p_other) {
		if (p_other.x == 0 || p_other.y == 0 || p_other.z == 0)
			VAR_ERR("zero division error");
		x /= (T)p_other.x; y /= (T)p_other.y; z /= (T)p_other.z;
		return *this;
	}

	/* cast operators */
	operator bool() const { return x == 0 && y == 0 && z == 0; }
	operator String() const { // FIXME
		return String("Vect3(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(", ")
			.append(std::to_string(z)).append(")");
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect2<T>& p_vect) {
	p_ostream << p_vect.operator String();
	return p_ostream;
}
template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect3<T>& p_vect) {
	p_ostream << p_vect.operator String();
	return p_ostream;
}

/* typedefs */
typedef Vect2<float> Vect2f;
typedef Vect2<int> Vect2i;
typedef Vect3<float> Vect3f;
typedef Vect3<int> Vect3i;

typedef Vect2f Size;
typedef Vect2f Point;

#endif //VECTOR_H

#ifndef  DICTIONARY_H
#define  DICTIONARY_H

//include "varhcore.h"

class var;
class String;

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

	/* wrappers */
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

	/* operators */
	operator bool() const { return empty(); }
	operator String() const;
	bool operator ==(const Dictionary& p_other) const;
};

#endif // DICTIONARY_H

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

#define DATA_MEM_SIZE 4 * sizeof(real_t)

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
		STRING,

		// math types
		VECT2F,
		VECT2I,
		VECT3F,
		VECT3I,

		// misc types
		ARRAY,
		DICTIONARY,
		OBJ_PTR,

		TYPE_MAX,
	};

private:
	static var tmp;
	Type type;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);
	template <typename T>
	friend bool _isinstance(const var& p_other);

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

	union VarData 
	{
		VarData() : _float(.0f) {}
		~VarData(){}
		Dictionary _dict;
		Array _arr;
		String _string;
		_DataObj _obj;

		bool _bool;
		int _int;
		double _float;
		uint8_t _mem[DATA_MEM_SIZE];
	};

	VarData _data;
	void copy_data(const var& p_other);
	void clear_data();

public:
	/* public api */
	inline Type get_type() const { return type; }
	String to_string() const { return operator String(); }
	void clear();
	var copy(bool p_deep = false) const;

	/* constructors */
	var();
	var(const var& p_copy);
	var(bool p_bool);
	var(int p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const String& p_string);
	var(const Vect2f& p_vect2f);
	var(const Vect2i& p_vect2i);
	var(const Vect3f& p_vect3f);
	var(const Vect3i& p_vect3i);
	var(const Array& p_array);
	var(const Dictionary& p_dict);

	template<typename T> var(const T& p_obj) {
		// this will cause a compile time error
		//static_assert(!std::is_enum<T>::value, "Do not use var<T>(const T&) with enums use (int)E_VAL instead");
		if (std::is_enum<T>::value){
			type = INT;
			_data._int = (int)p_obj;
		} else {
			type = OBJ_PTR;
			const void * _ptr = (const void *)&p_obj;
			_data._obj = _DataObj(typeid(T).hash_code(), typeid(T).name(), _ptr);
		}
	}

	template<typename T> var(const T* p_obj) {
		type = OBJ_PTR;
		const void* _ptr = (const void*)p_obj;
		_data._obj = _DataObj(typeid(T).hash_code(), typeid(T).name(), _ptr);
	}


	/* casting */
	operator bool() const;
	operator int() const;
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator String() const;
	// this treated as: built-in C++ operator[](const char *, int), conflict with operator[](size_t)
	// operator const char* () const;
	operator Vect2f() const;
	operator Vect2i() const;
	operator Vect3f() const;
	operator Vect3i() const;
	operator Array() const;
	operator Dictionary() const;

	template<typename T>
	T* as() const {
		switch (type) {
			// TODO: 
			case ARRAY: return (T*)(&_data._arr);
			case DICTIONARY: return (T*)(&_data._dict);
			case OBJ_PTR: return (T*)_data._obj._ptr;
		}
		VAR_ERR("invalid casting");
		return nullptr;
	}

	template<typename T>
	T as_enum() const {
		static_assert(std::is_enum<T>::value, "invalid use of as_enum on non enum type");
		if (type != INT) {
			VAR_ERR("cant cast non integer to enum");
		}
		return (T)_data._int;
	}

	bool is(const var& p_other) {
		switch (type) {
			case _NULL: return false;
			case BOOL:
			case INT:
			case FLOAT:
			case STRING:
			case VECT2F:
			case VECT2I:
			case VECT3F:
			case VECT3I:
				return operator ==(p_other);
			case ARRAY: return _data._arr._data == p_other._data._arr._data;
			case DICTIONARY: return _data._dict._data == p_other._data._dict._data;
			case OBJ_PTR: return _data._obj._ptr == p_other._data._obj._ptr;
		}
		VAR_ERR("invalid var type");
		return false;
	}

	/* operator overloading */
		/* comparison */
#define _VAR_OP_DECL(m_ret, m_op, m_access)                                                        \
	m_ret operator m_op (bool p_other) m_access { return operator m_op (var(p_other)); }           \
	m_ret operator m_op (int p_other) m_access { return operator m_op (var(p_other)); }            \
	m_ret operator m_op (float p_other) m_access { return operator m_op (var(p_other)); }          \
	m_ret operator m_op (double p_other) m_access { return operator m_op (var(p_other)); }         \
	m_ret operator m_op (const char* p_other) m_access { return operator m_op (var(p_other)); }    \
	m_ret operator m_op (const var& p_other) m_access
#define VAR_OP_DECL(m_ret, m_op, m_access) _VAR_OP_DECL(m_ret, m_op, m_access)

	VAR_OP_DECL(bool, ==, const);
	VAR_OP_DECL(bool, !=, const);
	VAR_OP_DECL(bool, < , const);
	VAR_OP_DECL(bool, > , const);
	VAR_OP_DECL(bool, <=, const);
	VAR_OP_DECL(bool, >=, const);

	//	/* unaray */
	var operator++();
	var operator++(int);
	var operator--();
	var operator--(int);
	bool operator !() const { return !operator bool(); }
	var& operator[](const var& p_key) const;

	//	/* binary */
	VAR_OP_DECL(var, +, const);
	VAR_OP_DECL(var, -, const);
	VAR_OP_DECL(var, *, const);
	VAR_OP_DECL(var, /, const);
	VAR_OP_DECL(var, %, const);

	//	/* assignments */
	var& operator=(const var& p_other);
	VAR_OP_DECL(var&, +=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, -=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, *=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, /=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, %=, PLACE_HOLDER_MACRO);

	~var();

};

#define isinstance(p_var, T) _isinstance<T>(p_var)
template<typename T>
bool _isinstance(const var& p_other) {
	switch (p_other.type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return false;
		case var::STRING: return typeid(p_other._data._string) == typeid(T);
		case var::VECT2F: return typeid(*DATA_PTR_OTHER_CONST(Vect2f)) == typeid(T);
		case var::VECT2I: return typeid(*DATA_PTR_OTHER_CONST(Vect2i)) == typeid(T);
		case var::VECT3F: return typeid(*DATA_PTR_OTHER_CONST(Vect3f)) == typeid(T);
		case var::VECT3I: return typeid(*DATA_PTR_OTHER_CONST(Vect3i)) == typeid(T);
		case var::ARRAY: return typeid(p_other._data._arr) == typeid(T);
		case var::DICTIONARY: return typeid(p_other._data._dict) == typeid(T);
		case var::OBJ_PTR: return p_other._data._obj.hash_code == typeid(T).hash_code();
	}
	VAR_ERR("invalid var type");
	return false;
}

// undefine all var.h macros defined in varcore.h
// this makes the user(carbon) independent of'em
#if defined(UNDEF_VAR_DEFINES)
#if !defined(VAR_H_HEADER_ONLY)

#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef newref_t1
#undef newref_t2
#undef DEBUG_BREAK
#undef VAR_ERR
#undef VAR_ASSERT
#undef UNDEF_VAR_DEFINES

#endif
#endif

#endif // _VAR_H

//--------------- VAR_IMPLEMENTATION -------------------

#if defined(VAR_IMPLEMENTATION)

//include "_var.h"

#define D_VEC(m_vect, m_dim, m_t) STRCAT3(m_vect, m_dim, m_t)

var var::tmp;

std::ostream& operator<<(std::ostream& p_ostream, const String& p_str) {
	p_ostream << p_str.c_str();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.operator String();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr) {
	p_ostream << p_arr.operator String();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Dictionary& p_dict) {
	p_ostream << p_dict.operator String();
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

// Array -----------------------------------------------
Array::operator String() const {
	std::stringstream ss;
	ss << "[ ";
	for (unsigned int i = 0; i < _data->size(); i++) {
		ss << _data->operator[](i).operator String();
		if (i != _data->size() - 1) ss << ", ";
		else ss << " ";
	}
	ss << "]";
	return ss.str();
}

bool Array::operator ==(const Array& p_other) const {
	if (size() != p_other.size())
		return false;
	for (size_t i = 0; i < size(); i++) {
		if (operator[](i) != p_other[i])
			return false;
	}
	return true;
}

Array Array::copy(bool p_deep) const {
	Array ret;
	for (size_t i = 0; i < size(); i++) {
		if (p_deep)
			ret.push_back(operator[](i).copy(true));
		else
			ret.push_back(operator[](i));
	}
	return ret;
}

Array Array::operator+(const Array& p_other) const {
	Array ret = copy();
	for (size_t i = 0; i < p_other.size(); i++) {
		ret.push_back(p_other[i].copy());
	}
	return ret;
}

Array& Array::operator+=(const Array& p_other) {
	for (size_t i = 0; i < p_other.size(); i++) {
		push_back(p_other[i].copy());
	}
	return *this;
}

// Dictionary ----------------------------------------
Dictionary::operator String() const {
	std::stringstream ss;
	ss << "{ ";
	for (std::map<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		ss << it->first.operator String() << " : " << it->second.operator String();
		ss << ", ";
	}
	ss << "}";
	return ss.str();
}

Dictionary Dictionary::copy(bool p_deep) const {
	Dictionary ret;
	for (std::map<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (p_deep)
			ret[it->first] = it->second.copy(true);
		else
			ret[it->first] = it->second;
	}
	return ret;
}

var& Dictionary::operator[](const var& p_key) const { return (*_data)[p_key]; }
var& Dictionary::operator[](const var& p_key) { return (*_data)[p_key]; }
std::map<var, var>::iterator Dictionary::begin() const { return (*_data).begin(); }
std::map<var, var>::iterator Dictionary::end() const { return (*_data).end(); }
std::map<var, var>::iterator Dictionary::find(const var& p_key) const { return (*_data).find(p_key); }

bool Dictionary::has(const var& p_key) const { return find(p_key) != end(); }

bool Dictionary::operator ==(const Dictionary& p_other) const {
	if (size() != p_other.size())
		return false;
	for (std::map<var, var>::iterator it_other = p_other.begin(); it_other != p_other.end(); it_other++) {
		std::map<var, var>::iterator it_self = find(it_other->first);
		if (it_self == end()) return false;
		if (it_self->second != it_other->second) return false;

	}
	return true;
}
// var -----------------------------------------------

void var::clear() {
	type = _NULL;
	clear_data();
}

var var::copy(bool p_deep) const {
	switch (type) {
		case _NULL:
		case BOOL:
		case INT:
		case FLOAT:
		case STRING:
		case VECT2F:
		case VECT2I:
		case VECT3F:
		case VECT3I:
			return *this;
		case ARRAY: return _data._arr.copy(p_deep);
		case DICTIONARY: return _data._dict.copy(p_deep);
		case OBJ_PTR:
			VAR_ERR("user defined objects are non copyable");
	}
	return *this;
}

/* constructors */
var::var() {
	_data._bool = false;
	type = _NULL;
}

var::var(const var& p_copy) {
	type = p_copy.type;
	copy_data(p_copy);
}

var::var(bool p_bool) {
	type = BOOL;
	_data._bool = p_bool;
}

var::var(int p_int) {
	type = INT;
	_data._int = p_int;
}

var::var(float p_float) {
	type = FLOAT;
	_data._float = p_float;
}

var::var(double p_double) {
	type = FLOAT;
	_data._float = p_double;
}

var::var(const char* p_cstring) {
	type = STRING;
	_data._string = String(p_cstring);
}

var::var(const String& p_string) {
	type = STRING;
	_data._string = p_string;
}

#define VAR_VECT_CONSTRUCTOR(m_dim, m_t, m_T)             \
var::var(const D_VEC(Vect, m_dim, m_t)& p_vect) {         \
	type = D_VEC(VECT, m_dim, m_T);                       \
	std::memcpy(_data._mem, &p_vect, sizeof(_data._mem)); \
}
VAR_VECT_CONSTRUCTOR(2, f, F)
VAR_VECT_CONSTRUCTOR(2, i, I)
VAR_VECT_CONSTRUCTOR(3, f, F)
VAR_VECT_CONSTRUCTOR(3, i, I)
#undef VAR_VECT_CONSTRUCTOR


var::var(const Array& p_array) {
	type = ARRAY;
	_data._arr = p_array._data;
}

var::var(const Dictionary& p_dict) {
	type = DICTIONARY;
	_data._dict = p_dict._data;
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)             \
var var::operator m_op () {                    \
	switch (type) {                            \
		case INT:  return m_op _data._int;     \
		case FLOAT: return m_op _data._float;  \
		default: VAR_ERR("invalid casting");   \
	}                                          \
	return var();                              \
}

#define VAR_OP_POST_INCR_DECR(m_op)            \
var var::operator m_op(int) {                  \
	switch (type) {                            \
		case INT: return _data._int m_op;      \
		case FLOAT: return _data._float m_op;  \
		default: VAR_ERR("invalid casting");   \
	}                                          \
	return var();                              \
}
VAR_OP_PRE_INCR_DECR(++)
VAR_OP_PRE_INCR_DECR(--)
VAR_OP_POST_INCR_DECR(++)
VAR_OP_POST_INCR_DECR(--)
#undef VAR_OP_PRE_INCR_DECR
#undef VAR_OP_POST_INCR_DECR

var& var::operator=(const var& p_other) {
	copy_data(p_other);
	return *this;
}

var& var::operator[](const var& p_key) const {
	switch (type) {
		case ARRAY: {
			int index = (int)p_key;
			if (0 <= index && index < (int)_data._arr.size())
				return _data._arr[index];
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				return _data._arr[_data._arr.size() + index];
			VAR_ERR("index error");
		}
		case DICTIONARY:
			return _data._dict[p_key];
	}
	VAR_ERR("invalid operator[]");
	return var::tmp;
}

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL: return false;
		case BOOL: return _data._bool;
		case INT: return _data._int != 0;
		case FLOAT: return _data._float != 0;
		case STRING: return _data._string.size() != 0;

		case VECT2F: return *DATA_PTR_CONST(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR_CONST(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case ARRAY: return !_data._arr.empty();
		case DICTIONARY: return !_data._dict.empty();
		case OBJ_PTR: return _data._obj._ptr == nullptr;
	}
	VAR_ERR("invalid casting");
	return false;
}

var::operator int() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		case STRING: return  _data._string.stoi();
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator double() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return _data._float;
		case STRING: return  _data._string.stod();
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator String() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return std::to_string(_data._int);
		case FLOAT: return std::to_string(_data._float);
		case STRING: return _data._string;
		case VECT2F: return (*DATA_PTR_CONST(Vect2f)).operator String();
		case VECT2I: return (*DATA_PTR_CONST(Vect2i)).operator String();
		case VECT3F: return (*DATA_PTR_CONST(Vect3f)).operator String();
		case VECT3I: return (*DATA_PTR_CONST(Vect3i)).operator String();
		case ARRAY: return _data._arr.operator String();
		case DICTIONARY: return _data._dict.operator String();
		case OBJ_PTR: return String("Object(").append(_data._obj.name).append(")");
	}
	VAR_ERR("invalid casting");
	return "TODO";
}

#define VAR_VECT_CAST(m_dim, m_t)                                                       \
var::operator D_VEC(Vect, m_dim, m_t)() const {                                         \
	switch (type) {                                                                     \
		case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, f));      \
		case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, i));      \
		default: VAR_ERR("invalid casting");                                            \
	}                                                                                   \
	return D_VEC(Vect, m_dim, m_t)();                                                   \
}
VAR_VECT_CAST(2, f)
VAR_VECT_CAST(2, i)
VAR_VECT_CAST(3, f)
VAR_VECT_CAST(3, i)
#undef VAR_VECT_CAST

var::operator Array() const {
	switch (type) {
		case ARRAY: return _data._arr;
		default: VAR_ERR("invalid casting");
	}
	return Array();
}

var::operator Dictionary() const {
	switch (type) {
		case DICTIONARY: return _data._dict;
		default: VAR_ERR("invalid casting");
	}
	return Dictionary();
}

/* operator overloading */
		/* comparison */
#define VAR_RET_OP(m_op, m_data1, m_type, m_data2, m_cast) \
	case m_type: return (m_cast)_data.m_data1 m_op p_other._data.m_data2
#define VAR_SWITCH_PRIME_TYPES(m_data, m_op)                              \
	switch (p_other.type) {                                               \
		VAR_RET_OP(m_op, m_data, BOOL, _bool, bool);                      \
		VAR_RET_OP(m_op, m_data, INT, _int, int);                         \
		VAR_RET_OP(m_op, m_data, FLOAT, _float, float);                   \
	}
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                           \
switch (p_other.type) {                                                                                                             \
	case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f));  \
	case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i));  \
}                                                                                                                                   \
break;

bool var::operator==(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, ==) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, ==) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, ==) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string == p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, ==) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, ==) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, ==) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, ==) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr == p_other.operator Array();
			}
			break;
		}
		case DICTIONARY: {
			if (p_other.type == DICTIONARY) {
				return _data._dict == p_other.operator Dictionary();
			}
			break;
		}
		case OBJ_PTR: {
			if (p_other.type == OBJ_PTR)
				return _data._obj._ptr == p_other._data._obj._ptr;
			break;
		}
	}
	return false;
}

bool var::operator!=(const var& p_other) const {
	return !operator==(p_other);
}

bool var::operator<(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, <) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, <) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, <) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, < ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, < ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, < ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, < ) }
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() < *p_other.operator Array().get_data();
			break;
		}
		case DICTIONARY:
		case OBJ_PTR: {
			if (p_other.type == OBJ_PTR)
				return _data._obj._ptr < p_other._data._obj._ptr;
			break;
		}
	}
	VAR_ERR("invalid < comparison");
	return false;
}

bool var::operator>(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, > ) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, > ) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, > ) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, > ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, > ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, > ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, > ) }
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() > *p_other.operator Array().get_data();
			break;
		}
		case DICTIONARY:
		case OBJ_PTR: {
			if (p_other.type == OBJ_PTR)
				return _data._obj._ptr > p_other._data._obj._ptr;
			break;
		}
	}
	VAR_ERR("invalid > comparison");
	return false;
}

bool var::operator<=(const var& p_other) const {
	return *this == p_other || *this < p_other;
}
bool var::operator>=(const var& p_other) const {
	return *this == p_other || *this > p_other;
}

var var::operator +(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, + ) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, + ) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, + ) }
		case STRING: {
			if (p_other.type == STRING)
				return _data._string + p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, + ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, + ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, + ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, + ) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr + p_other._data._arr;
			}
			break;
		}
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator +");
	return false;
}

var var::operator -(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, -) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, -) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, -) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, -) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, -) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, -) }
		case ARRAY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator -");
	return false;
}

var var::operator *(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, *) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, *) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, *) }
		case STRING:
			break; // TODO: maybe python like
		case VECT2F: { VAR_SWITCH_VECT(2, f, *) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, *) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, *) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, *) }
		case ARRAY:
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator *");
	return false;
}

#define CASE_DIV_DATA(m_data1, m_data2)                                   \
{                                                                         \
if (m_data2 == 0)                                                         \
	VAR_ERR("zero division error");                                       \
return m_data1 / m_data2;                                                 \
}

#define SWITCH_DIV_TYPES(m_data, m_cast)                                  \
switch (p_other.type) {                                                   \
	case BOOL:                                                            \
		CASE_DIV_DATA(m_cast _data.m_data, (int)p_other._data._bool)      \
	case INT:                                                             \
		CASE_DIV_DATA(m_cast _data.m_data, p_other._data._int)            \
	case FLOAT:                                                           \
		CASE_DIV_DATA(m_cast _data.m_data, p_other._data._float)          \
}

var var::operator /(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: { SWITCH_DIV_TYPES(_bool, (int)) }
		case INT: { SWITCH_DIV_TYPES(_int, +) }
		case FLOAT: { SWITCH_DIV_TYPES(_float , +) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, /) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, /) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, /) }
		case ARRAY:
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator /");
	return false;
}
#undef SWITCH_DIV_TYPES
#undef CASE_DIV_DATA

var var::operator %(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: {
			switch (p_other.type) {
				case BOOL: return (int)(_data._bool) % (int)(p_other._data._bool);
				case INT: return (int)_data._bool % p_other._data._int;
			}
		}
		case INT: {
			switch (p_other.type) {
				case BOOL: return _data._int % (int)(p_other._data._bool);
				case INT: return _data._int % p_other._data._int;
			}
		}
	}
	VAR_ERR("unsupported operator %");
	return false;
}
#undef VAR_RET_EQUAL
#undef VAR_SWITCH_PRIME_TYPES
#undef VAR_SWITCH_VECT
#undef SWITCH_DIV_TYPES
#undef CASE_DIV_DATA


#define VAR_CASE_OP(m_op, m_data1, m_type, m_data2, m_cast) \
	case m_type: _data.m_data1 m_op (m_cast)p_other._data.m_data2; return *this
#define VAR_SWITCH_PRIME_TYPES(m_data, m_cast, m_op)                               \
	switch (p_other.type) {                                                        \
		VAR_CASE_OP(m_op, m_data, BOOL, _bool , m_cast);                           \
		VAR_CASE_OP(m_op, m_data, INT, _int, m_cast);                              \
		VAR_CASE_OP(m_op, m_data, FLOAT, _float, m_cast);                          \
	}
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                               \
switch (p_other.type) {                                                                                                                 \
	case D_VEC(VECT, m_dim, F): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f)); return *this;     \
	case D_VEC(VECT, m_dim, I): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i)); return *this;     \
}                                                                                                                                       \
break;
var& var::operator+=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, int, +=) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, int, +=) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, +=) }
		case STRING: {
			if (p_other.type == STRING) {
				_data._string += p_other._data._string;
				return *this;
			}
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, +=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, +=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, +=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, +=) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				_data._arr += p_other._data._arr;
				return *this;
			}
			break;
		}
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator +=");
	return *this;
}

var& var::operator-=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL : { VAR_SWITCH_PRIME_TYPES(_bool, int, -=) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, int, -=) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, -=) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, -=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, -=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, -=) }
		case ARRAY:
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator -=");
	return *this;
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: { VAR_SWITCH_PRIME_TYPES(_bool, int, *=) }
		case INT: { VAR_SWITCH_PRIME_TYPES(_int, int, *=) }
		case FLOAT: { VAR_SWITCH_PRIME_TYPES(_float, float, *=) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, *=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, *=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, *=) }
		case ARRAY:
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator *=");
	return *this;
}

#define CASE_DIV_DATA(m_data1, m_data2)                                   \
{                                                                         \
if (m_data2 == 0)                                                         \
	VAR_ERR("zero division error");                                       \
m_data1 /= m_data2;                                                       \
return *this;                                                             \
}

#define SWITCH_DIV_TYPES(m_data, m_cast)                                  \
switch (p_other.type) {                                                   \
	case BOOL:                                                            \
		CASE_DIV_DATA(_data.m_data, (int)p_other._data._bool)             \
	case INT:                                                             \
		CASE_DIV_DATA(_data.m_data, (m_cast)p_other._data._int)           \
	case FLOAT:                                                           \
		CASE_DIV_DATA(_data.m_data, (m_cast)p_other._data._float)         \
}
var& var::operator/=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: { SWITCH_DIV_TYPES(_bool, bool) }
		case INT: { SWITCH_DIV_TYPES(_int, int) }
		case FLOAT: { SWITCH_DIV_TYPES(_float, float) }
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, /=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, /=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, /=) }
		case ARRAY:
		case DICTIONARY:
		case OBJ_PTR:
			break;
	}
	VAR_ERR("unsupported operator /=");
	return *this;
}

var& var::operator %=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: break;
		case INT: {
			switch (p_other.type) {
				//case BOOL: _data._int %= (int)(p_other._data._bool); return *this;
				case INT: _data._int %= p_other._data._int; return *this;
			}
		}
	}
	VAR_ERR("unsupported operator %=");
	return *this;
}

void var::copy_data(const var& p_other) {
	clear_data();
	switch (p_other.type) {
		case var::_NULL: return;
		case var::BOOL:
			_data._bool = p_other._data._bool;
			break;
		case var::INT:
			_data._int = p_other._data._int;
			break;
		case var::FLOAT:
			_data._float = p_other._data._float;
			break;
		case var::STRING:
			_data._string = p_other._data._string;
			break;
		case var::VECT2F:
		case var::VECT2I:
		case var::VECT3F:
		case var::VECT3I:
			for (int i = 0; i < DATA_MEM_SIZE; i++) {
				_data._mem[i] = p_other._data._mem[i];
			}
			break;
		case var::ARRAY:
			_data._arr = p_other._data._arr;
			break;
		case var::DICTIONARY:
			_data._dict = p_other._data._dict;
			break;
		case var::OBJ_PTR:
			_data._obj = p_other._data._obj;
			return;
	}
}

void var::clear_data() {
	switch (type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
		case var::STRING:
		case var::VECT2F:
		case var::VECT2I:
		case var::VECT3F:
		case var::VECT3I:
			return;
		case var::ARRAY:
			_data._arr._data = nullptr;
		case var::DICTIONARY:
			_data._dict._data = nullptr;
		case var::OBJ_PTR:
			_data._obj._ptr = nullptr; // may cause memory leak
	}
}

#undef VAR_CASE_OP
#undef VAR_SWITCH_PRIME_TYPES
#undef VAR_SWITCH_VECT
#undef CASE_DIV_DATA
#undef SWITCH_DIV_TYPES

#if defined(UNDEF_VAR_DEFINES)
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef newref_t1
#undef newref_t2
#undef DEBUG_BREAK
#undef VAR_ERR
#undef VAR_ASSERT
#undef UNDEF_VAR_DEFINES
#endif


#endif // VAR_IMPLEMENTATION