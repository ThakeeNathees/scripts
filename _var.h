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

#include "_string.h"
#include "_array.h"
#include "_vector.h"
#include "_dictionary.h"
#include "_object.h"

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

#define DATA_MEM_SIZE 4 * sizeof(real_t)

// TODO: var fn = &func; fn(); operator(){}

namespace varh {

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
		OBJECT,
		//OBJ_PTR,

		TYPE_MAX,
	};

private:
	static var tmp;
	Type type;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);

	struct VarData
	{
		VarData() : _float(.0f) {}
		~VarData(){}

		Dictionary _dict;
		Array _arr;
		Ref<Object> _obj;

		union {
			String _string;

			bool _bool;
			int _int;
			double _float;
			uint8_t _mem[DATA_MEM_SIZE];
		};
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

	template<typename T> var(const T& p_enum) {
		static_assert(!std::is_enum<T>::value, "Use var<T>(const T&) only with enums");
		type = INT;
		_data._int = (int)p_enum;
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

	// make Array Dictionary String as object and use cast<T>()
	//template<typename T>
	//T* as() const {
	//	switch (type) {
	//		// TODO: 
	//		case ARRAY: return (T*)(&_data._arr);
	//		case DICTIONARY: return (T*)(&_data._dict);
	//		case OBJECT: return (T*)_data._obj._ptr;
	//	}
	//	VAR_ERR("invalid casting");
	//	return nullptr;
	//}

	template<typename T>
	T as_enum() const {
		static_assert(std::is_enum<T>::value, "Invalid use of as_enum<T>() on non enum type");
		if (type != INT) {
			VAR_ERR("cant cast non integer to enum");
		}
		return (T)_data._int;
	}

	//bool is(const var& p_other) {
	//	switch (type) {
	//		case _NULL: return false;
	//		case BOOL:
	//		case INT:
	//		case FLOAT:
	//		case STRING:
	//		case VECT2F:
	//		case VECT2I:
	//		case VECT3F:
	//		case VECT3I:
	//			return operator ==(p_other);
	//		case ARRAY: return _data._arr._data == p_other._data._arr._data;
	//		case DICTIONARY: return _data._dict._data == p_other._data._dict._data;
	//		case OBJECT: return *_data._obj == *p_other._data._obj;
	//	}
	//	VAR_ERR("invalid var type");
	//	return false;
	//}

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