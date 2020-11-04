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
#include "_map.h"
#include "_object.h"
#include "_vector.h" // not a part of var anymore

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

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
	var(size_t p_int);
	var(int64_t p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const String& p_string);
	var(const Array& p_array);
	var(const Map& p_map);
	~var();
	
	template <typename T=Object>
	var(const ptr<T>& p_ptr) {
		type = OBJECT;
		new(&_data._obj) ptr<Object>(p_ptr);
	}

	template <typename... Targs>
	var operator()(Targs... p_args) {
		return __call(p_args...);
	}

	// Methods.
	inline Type get_type() const { return type; }
	String get_type_name() const;
	//const char* get_parent_class_name() const;
	size_t hash() const;
	static bool is_hashable(var::Type p_type);
	void clear();
	var copy(bool p_deep = false) const;

	constexpr static const char* get_type_name_s(var::Type p_type) {
		switch (p_type) {
			case var::_NULL:  return "null";
			case var::VAR:    return "var";
			case var::BOOL:   return "bool";
			case var::INT:    return "int";
			case var::FLOAT:  return "float";
			case var::STRING: return "String";
			case var::ARRAY:  return "Array";
			case var::MAP:    return "Map";
			case var::OBJECT: return "Object";
			default:
				return "";
		}
	}

	// Operators.
	operator bool() const;
	operator int64_t() const;
	operator int() const { return (int)operator int64_t(); }
	operator size_t() const { return (size_t)operator int64_t(); }
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator String() const;   // int.operator String() is invalid casting.
	String to_string() const;  // int.to_string() is valid.
	// this treated as: built-in C++ operator[](const char *, int), conflict with operator[](size_t)
	// operator const char* () const;
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

	var __iter_begin();

	template <typename... Targs>
	var __call(Targs... p_args) {
		stdvec<var> args = make_stdvec<var>(p_args...);
		return __call_internal(args);
	}
	template <typename... Targs>
	var call_method(const String& p_method, Targs... p_args) {
		stdvec<var> args = make_stdvec<var>(p_args...);
		return call_method_internal(p_method, args);
	}
	var call_method(const String& p_method, stdvec<var>& p_args) { return call_method_internal(p_method, p_args); }

	var get_member(const String& p_name);
	void set_member(const String& p_name, var& p_value);

	const MemberInfo* get_member_info(const String& p_name) const;
	static const MemberInfo* get_member_info_s(var::Type p_type, const String& p_name);
	const stdvec<const MemberInfo*> get_member_info_list() const;
	static stdvec<const MemberInfo*> get_member_info_list_s(var::Type p_type);

private:
	var __call_internal(stdvec<var>& p_args);
	var call_method_internal(const String& p_method, stdvec<var>& p_args);
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


		union {
			ptr<Object> _obj;
			Map _map;
			Array _arr;
			String _string;

			bool _bool = false;
			int64_t _int;
			double _float;
		};
	};

	// Methods.
	void copy_data(const var& p_other);
	void clear_data();

	// Members.
	static var tmp;
	Type type = _NULL;
	VarData _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);
};

// ******** MEMBER INFO IMPLEMENTATIONS ******************* //

struct VarTypeInfo {
	var::Type type = var::_NULL;
	const char* class_name = nullptr;
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
	int arg_count = 0; // -1 is va args, -2 is unknown
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


/////////////////////////////////////////////////////////////////

struct Map::_KeyValue {
	var key;
	var value;
	_KeyValue() {}
	_KeyValue(const var& p_key, const var& p_value) : key(p_key), value(p_value) {}
};

}


// include _native after including everything else.
#include "_native.h"
#include "_iterator.h"

// undefine all var.h macros defined in varcore.h
// this makes the user(carbon) independent of'em
#if defined(UNDEF_VAR_DEFINES)
#if !defined(VAR_H_HEADER_ONLY)

#undef func
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