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
#include "_dictionary.h"

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)
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

	std::string _data_std_string;
	Array _data_arr;
	Dictionary _data_dict;
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
	std::string to_string() const { return operator std::string(); }
	void clear();
	var copy(bool p_deep = false) const;

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
	//var(const Dictionary& p_dict);
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
	operator const char* () const;
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

	bool is(const var& p_other) {
		switch (type) {
			case _NULL: return false;
			case BOOL:
			case INT:
			case FLOAT:
			case STD_STRING:
			case VECT2F:
			case VECT2I:
			case VECT3F:
			case VECT3I:
				return operator ==(p_other);
			case ARRAY: return _data_arr._data == p_other._data_arr._data;
			case OBJ_PTR: return _data_obj._ptr == p_other._data_obj._ptr;
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
	var& operator[](size_t index);
	var& operator[](size_t index) const;

	//	/* binary */
	VAR_OP_DECL(var, +, const);
	VAR_OP_DECL(var, -, const);
	VAR_OP_DECL(var, *, const);
	VAR_OP_DECL(var, /, const);
	VAR_OP_DECL(var, %, const);

	//	/* assignments */
	var& operator=(const var& p_other) = default;
	VAR_OP_DECL(var&, +=, M_PLACE_HOLDER);
	VAR_OP_DECL(var&, -=, M_PLACE_HOLDER);
	VAR_OP_DECL(var&, *=, M_PLACE_HOLDER);
	VAR_OP_DECL(var&, /=, M_PLACE_HOLDER);
	VAR_OP_DECL(var&, %=, M_PLACE_HOLDER);

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
		case var::STD_STRING: return typeid(p_other._data_std_string) == typeid(T);
		case var::VECT2F: return typeid(*DATA_PTR_OTHER_CONST(Vect2f)) == typeid(T);
		case var::VECT2I: return typeid(*DATA_PTR_OTHER_CONST(Vect2i)) == typeid(T);
		case var::VECT3F: return typeid(*DATA_PTR_OTHER_CONST(Vect3f)) == typeid(T);
		case var::VECT3I: return typeid(*DATA_PTR_OTHER_CONST(Vect3i)) == typeid(T);
		case var::ARRAY: return typeid(p_other._data_arr) == typeid(T);
		case var::OBJ_PTR: return p_other._data_obj.hash_code == typeid(T).hash_code();
	}
	VAR_ERR("invalid var type");
	return false;
}

#endif // _VAR_H