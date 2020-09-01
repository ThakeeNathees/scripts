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
#include "_map.h"
#include "_object.h"

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

#define DATA_MEM_SIZE 4 * sizeof(real_t)

// TODO: var fn = &func; fn(); operator(){}

namespace varh {

class var {
public:
	enum Type {
		_NULL, // not initialized.
		VAR,   // any type used only for member info.

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
		MAP,
		OBJECT,

		_TYPE_MAX_,
	};

	/* constructors */
	var();
	var(const var& p_copy);
	var(bool p_bool);
	var(int p_int);
	var(int64_t p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const String& p_string);
	var(const Vect2f& p_vect2f);
	var(const Vect2i& p_vect2i);
	var(const Vect3f& p_vect3f);
	var(const Vect3i& p_vect3i);
	var(const Array& p_array);
	var(const Map& p_map);
	~var();
	
	template <typename T=Object>
	var(const ptr<T>& p_ptr) {
		type = OBJECT;
		_data._obj = p_ptr;
	}

	// Methods.
	inline Type get_type() const { return type; }
	static String get_type_name_s(var::Type p_type);
	String get_type_name() const;
	//const char* get_parent_class_name() const;
	size_t hash() const;
	void clear();
	var copy(bool p_deep = false) const;

	// Operators.
	operator bool() const;
	operator int64_t() const;
	operator int() const { return (int)operator int64_t(); }
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator String() const;   // int.operator String() is invalid casting.
	String to_string() const;  // int.to_string() is valid.
	// this treated as: built-in C++ operator[](const char *, int), conflict with operator[](size_t)
	// operator const char* () const;
	operator Vect2f() const;
	operator Vect2i() const;
	operator Vect3f() const;
	operator Vect3i() const;
	operator Array() const;
	operator Map() const;
	operator ptr<Object>() const;

	template <typename T>
	ptr<T> cast_to() const {
		return ptrcast<T>(operator ptr<Object>());
	}

#define _VAR_OP_DECL(m_ret, m_op, m_access)                                                        \
	m_ret operator m_op (bool p_other) m_access { return operator m_op (var(p_other)); }           \
	m_ret operator m_op (int64_t p_other) m_access { return operator m_op (var(p_other)); }        \
	m_ret operator m_op (int p_other)     m_access { return operator m_op (var(p_other)); }        \
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

	var operator++();
	var operator++(int);
	var operator--();
	var operator--(int);
	bool operator !() const { return !operator bool(); }
	var operator[](const var& p_key) const;
	// var& operator[](const var& p_key); // TODO:

	var __get_mapped(const var& p_key) const;
	void __set_mapped(const var& p_key, const var& p_value);

	template <typename... Targs>
	var __call(Targs... p_args) {
		stdvec<var> args;
		__build_args_recur(args, p_args...);
		return __call_internal(args);
	}
	template <typename... Targs>
	var call_method(const String& p_method, Targs... p_args) {
		stdvec<var> args;
		__build_args_recur(args, p_args...);
		return call_method_internal(p_method, args);
	}
	var call_method(const String& p_method, stdvec<var>& p_args) { return call_method_internal(p_method, p_args); }

	var get_member(const String& p_name);
	void set_member(const String& p_name, var& p_value);

private:
	var __call_internal(stdvec<var>& p_args);
	// TODO: call static func in every var classes.
	var call_method_internal(const String& p_method, stdvec<var>& p_args);
	template <typename T, typename... Targs>
	constexpr void __build_args_recur(stdvec<var>& p_args_arr, T p_val, Targs... p_args) {
		p_args_arr.push_back(p_val);
		__build_args_recur(p_args_arr, p_args...);
	}
	void __build_args_recur(stdvec<var>& p_args_arr) { return; }
public:

	VAR_OP_DECL(var, +, const);
	VAR_OP_DECL(var, -, const);
	VAR_OP_DECL(var, *, const);
	VAR_OP_DECL(var, /, const);
	VAR_OP_DECL(var, %, const);

	/* assignments */
	var& operator=(const var& p_other);
	template<typename T=Object>
	var& operator=(const ptr<T>& p_other) {
		clear_data();
		type = OBJECT;
		_data._obj = ptrcast<Object>(p_other);
		return *this;
	}
	VAR_OP_DECL(var&, +=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, -=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, *=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, /=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, %=, PLACE_HOLDER_MACRO);

private:
	struct VarData {
		VarData() : _float(.0f) {}
		~VarData() {}

		Map _map;
		Array _arr;
		ptr<Object> _obj;

		union {
			String _string;

			bool _bool;
			int64_t _int;
			double _float;
			uint8_t _mem[DATA_MEM_SIZE];
		};
	};

	// Methods.
	void copy_data(const var& p_other);
	void clear_data();

	// Members.
	static var tmp;
	Type type;
	VarData _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);
};

// ******** MEMBER INFO IMPLEMENTATIONS ******************* //

struct VarTypeInfo {
	var::Type type;
	const char* class_name;
	VarTypeInfo(var::Type p_type = var::VAR) : type(p_type) {}
	VarTypeInfo(var::Type p_type, const char* p_class_name) : type(p_type), class_name(p_class_name) {}

	bool operator==(const VarTypeInfo p_other) const {
		if (type != var::OBJECT) return type == p_other.type;
		return strcmp(class_name, p_other.class_name) == 0;
	}
	bool operator!=(const VarTypeInfo p_other) const { return !(operator==(p_other)); }
};

class MethodInfo : public MemberInfo {
private:
	String name;
	bool _is_static = false;
	int arg_count = 0; // -1 means static
	stdvec<String> arg_names;
	stdvec<var> default_args;
	stdvec<VarTypeInfo> arg_types;
	VarTypeInfo return_type;

public:
	virtual Type get_type() const override { return Type::METHOD; }
	virtual const String& get_name() const override { return name; }

	// complete constructor.
	MethodInfo(
		String p_name,
		stdvec<String> p_arg_names,
		stdvec<VarTypeInfo> p_arg_types = stdvec<VarTypeInfo>(),
		VarTypeInfo p_return_type = var::_NULL,
		bool p__is_static = false,
		stdvec<var> p_default_args = stdvec<var>(),
		int p_arg_count = -2
	) {
		name = p_name;
		arg_names = p_arg_names;
		arg_types = p_arg_types;
		return_type = p_return_type;
		_is_static = p__is_static;
		default_args = p_default_args;

		// if va_arg it should be set manually to -1
		if (p_arg_count == -2) arg_count = (int)arg_names.size();
		else arg_count = p_arg_count;
	}

	// zero parameter constructor
	MethodInfo(
		String p_name,
		VarTypeInfo p_return_type = var::_NULL,
		bool p__is_static = false
	) {
		name = p_name;
		return_type = p_return_type;
		_is_static = p__is_static;
	}

	int get_arg_count() const { return arg_count; }
	int get_default_arg_count() const { return (int)default_args.size(); }
	bool is_static() const { return _is_static; }
	const stdvec<String>& get_arg_names() const { return arg_names; }
	const stdvec<var>& get_default_args() const { return default_args; }
	const stdvec<VarTypeInfo>& get_arg_types() const { return arg_types; }
	VarTypeInfo get_return_type() const { return return_type; }
};

class PropertyInfo : public MemberInfo {
private:
	String name;
	VarTypeInfo datatype;
	var value;
	bool _is_const = false;
	bool _is_static = false;

public:
	virtual Type get_type() const override { return Type::PROPERTY; }
	virtual const String& get_name() const override { return name; }

	PropertyInfo(
		const String& p_name,
		VarTypeInfo p_datatype = var::VAR,
		var p_value = var(),
		bool p__is_const = false,
		bool p__is_static = false
	) {
		name = p_name;
		datatype = p_datatype;
		value = p_value;
		_is_const = p__is_const;
		_is_static = p__is_static;
	}

	bool is_static() const { return _is_static; }
	bool is_const() const { return _is_const; }
	VarTypeInfo get_datatype() const { return datatype; }
	const var& get_value() const { return value; }         // value for constants.
	const var& get_default_value() const { return value; } // defalut_value for variables.
};

class EnumInfo : public MemberInfo {
private:
	String name;
	stdvec<std::pair<String, int64_t>> values;

public:
	virtual Type get_type() const override { return Type::ENUM; }
	virtual const String& get_name() const override { return name; }

	EnumInfo(String p_name, const stdvec<std::pair<String, int64_t>>& p_values) {
		name = p_name;
		values = p_values;
	}

	const stdvec<std::pair<String, int64_t>>& get_values() const { return values; }
};

class EnumValueInfo : public MemberInfo {
private:
	String name;
	int64_t value;

public:
	virtual Type get_type() const override { return Type::ENUM_VALUE; }
	virtual const String& get_name() const override { return name; }

	EnumValueInfo(const String& p_name, int64_t p_value) {
		name = p_name;
		value = p_value;
	}

	int64_t get_value() const { return value; }
};

}

// undefine all var.h macros defined in varcore.h
// this makes the user(carbon) independent of'em
#if defined(UNDEF_VAR_DEFINES)
#if !defined(VAR_H_HEADER_ONLY)

#undef func
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_PRINT
#undef DEBUG_BREAK
#undef VAR_ASSERT
#undef MISSED_ENUM_CHECK
#undef UNDEF_VAR_DEFINES

#endif
#endif

#endif // _VAR_H