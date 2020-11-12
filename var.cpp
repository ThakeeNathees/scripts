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

#include "_var.h"
#include <functional>

namespace varh {

var var::tmp;

std::ostream& operator<<(std::ostream& p_ostream, const String& p_str) {
	p_ostream << p_str.operator std::string();
	return p_ostream;
}
std::istream& operator>>(std::istream& p_istream, String& p_str) {
	p_istream >> *p_str._data;
	return p_istream;
}

std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.to_string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr) {
	p_ostream << p_arr.to_string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Map& p_map) {
	p_ostream << p_map.to_string();
	return p_ostream;
}

static const char* _error_names[VarError::_ERROR_MAX_] = {
	"OK",
	"BUG",
	"NULL_POINTER",
	"OPERATOR_NOT_SUPPORTED",
	"NOT_IMPLEMENTED",
	"ZERO_DIVISION",
	"TYPE_ERROR",
	"ATTRIBUTE_ERROR",
	"INVALID_ARG_COUNT",
	"INVALID_INDEX",
	//_ERROR_MAX_
};
MISSED_ENUM_CHECK(VarError::_ERROR_MAX_, 10);

std::string VarError::get_err_name(VarError::Type p_type) {
	return _error_names[p_type];
}

#define CHECK_METHOD_AND_ARGS()                                                                                                                         \
do {                                                                                                                                                    \
	if (has_member(p_method)) {                                                                                                                         \
		if (get_member_info(p_method)->get_type() != MemberInfo::METHOD)                                                                                \
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("member \"%s\" is not callable.", p_method.c_str()));                                   \
		const MethodInfo* mp = (MethodInfo*)get_member_info(p_method);                                                                                  \
		int arg_count = mp->get_arg_count();                                                                                                            \
		int default_arg_count = mp->get_default_arg_count();                                                                                            \
		if (arg_count != -1) {                                                                                                                          \
			if (p_args.size() + default_arg_count < arg_count) { /* Args not enough. */                                                                 \
				if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at exactly %i argument(s).", arg_count)); \
				else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at least %i argument(s).", arg_count - default_arg_count));      \
			} else if (p_args.size() > arg_count) { /* More args proveded.    */                                                                           \
				if (default_arg_count == 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format("expected at exactly %i argument(s).", arg_count)); \
				else THROW_VARERROR(VarError::INVALID_ARG_COUNT, String::format(                                                                           \
					"expected minimum of %i argument(s) and maximum of %i argument(s).", arg_count - default_arg_count, arg_count));                    \
			}                                                                                                                                           \
		}                                                                                                                                               \
		for (int j = 0; j < mp->get_arg_types().size(); j++) {                                                                                          \
			if (mp->get_arg_types()[j] == VarTypeInfo(var::VAR)) continue; /* can't be _NULL. */                                                        \
			if (p_args.size() == j) break; /* rest are default args. */                                                                                 \
			if (mp->get_arg_types()[j] != VarTypeInfo(p_args[j]->get_type(), p_args[j]->get_type_name().c_str()))                                       \
				THROW_VARERROR(VarError::TYPE_ERROR, String::format(                                                                                    \
					"expected type %s at argument %i.", var::get_type_name_s(mp->get_arg_types()[j].type), j));                                         \
		}                                                                                                                                               \
	} else {                                                                                                                                            \
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" not exists on base %s.", p_method.c_str(), get_class_name_s()));     \
	}                                                                                                                                                   \
} while (false)

#define MEMBER_INFO_IMPLEMENTATION(m_type)                                                                                                \
bool m_type::has_member(const String& p_member) {                                                                                         \
	return get_member_info_list().find(p_member) != get_member_info_list().end();                                                         \
}                                                                                                                                         \
const MemberInfo* m_type::get_member_info(const String& p_member) {                                                                       \
	if (!has_member(p_member))                                                                                                            \
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" not exists on base " #m_type ".", p_member.c_str()));  \
	const stdmap<String, const MemberInfo*>& member_info = get_member_info_list();                                                        \
	return member_info.at(p_member);                                                                                                      \
}                                                                                                                                         \
const stdmap<String, const MemberInfo*>& m_type::get_member_info_list()

	// TODO: eventhought the method should be alive all the time it needs to be cleaned
	//       delete or use shared pointers.
#define MEMBER_INFO(m_name, ...) { m_name,      new MethodInfo(m_name, __VA_ARGS__) }

// String -----------------------------------------------

MEMBER_INFO_IMPLEMENTATION(String) {
	static const stdmap<String, const MemberInfo*> member_info = {
		MEMBER_INFO("size",                                               var::INT),
		MEMBER_INFO("length",                                             var::INT),
		MEMBER_INFO("to_int",                                             var::INT),
		MEMBER_INFO("to_float",                                           var::FLOAT),
		MEMBER_INFO("get_line",   {"line"}, {var::INT},                   var::STRING),
		MEMBER_INFO("hash",                                               var::INT),
		MEMBER_INFO("substr",     {"start", "end"}, {var::INT, var::INT}, var::STRING),
		MEMBER_INFO("endswith",   {"what"}, {var::STRING},                var::BOOL),
		MEMBER_INFO("startswith", {"what"}, {var::STRING},                var::BOOL),
	};
	return member_info;
}

var String::call_method(const String& p_method, const stdvec<var*>& p_args) {

	CHECK_METHOD_AND_ARGS();

	switch (p_method.const_hash()) {
		case "size"_hash:        // [[fallthrough]]
		case "length"_hash:      return (int64_t)size();
		case "to_int"_hash:     return to_int();
		case "to_float"_hash:   return to_float();
		case "get_line"_hash:   return get_line(p_args[0]->operator int64_t());
		case "hash"_hash:       return (int64_t)hash();
		case "substr"_hash:     return substr((size_t)p_args[0]->operator int64_t(), (size_t)p_args[1]->operator int64_t());
		case "endswith"_hash:   return endswith(p_args[0]->operator String());
		case "startswith"_hash: return startswith(p_args[0]->operator String());
	}
	// TODO: more.
	THROW_VARERROR(VarError::BUG, "can't reach here.");
}

String String::format(const char* p_format, ...) {
	va_list argp;

	va_start(argp, p_format);

	static const unsigned int BUFFER_SIZE = VSNPRINTF_BUFF_SIZE;
	char buffer[BUFFER_SIZE + 1]; // +1 for the terminating character
	int len = vsnprintf(buffer, BUFFER_SIZE, p_format, argp);

	va_end(argp);

	if (len == 0) return String();
	return String(buffer);
}

String String::get_line(uint64_t p_line) const {
	const char* source = _data->c_str();
	uint64_t cur_line = 1;
	std::stringstream ss_line;

	while (char c = *source) {
		if (c == '\n') {
			if (cur_line >= p_line) break;
			cur_line++;
		} else if (cur_line == p_line) {
			ss_line << c;
		}
		source++;
	}

	ss_line << '\n';
	return ss_line.str();
}

String String::substr(size_t p_start, size_t p_end) const {
	return _data->substr(p_start, p_end - p_start);
}
bool String::endswith(const String& p_str) const {
	if (p_str.size() > _data->size()) return false;
	for (size_t i = 1; i <= p_str.size(); i++) {
		if ((*_data)[_data->size() - i] != p_str[p_str.size() - i]) {
			return false;
		}
	}
	return true;
}
bool String::startswith(const String& p_str) const {
	if (p_str.size() > _data->size()) return false;
	for (size_t i = 0; i < p_str.size(); i++) {
		if ((*_data)[i] != p_str[i]) {
			return false;
		}
	}
	return true;
}

bool operator==(const char* p_cstr, const String& p_str) {
	return p_str == String(p_cstr);
}
bool operator!=(const char* p_cstr, const String& p_str) {
	return p_str != String(p_cstr);
}

// Array -----------------------------------------------

MEMBER_INFO_IMPLEMENTATION(Array) {
	static const stdmap<String, const MemberInfo*> member_info = {
		MEMBER_INFO("size",                                   var::INT),
		MEMBER_INFO("empty",                                  var::BOOL),
		MEMBER_INFO("push_back", {"element"}, {var::VAR},     var::_NULL),
		MEMBER_INFO("pop_back",                               var::_NULL),
		MEMBER_INFO("append",    {"element"}, {var::VAR},     var::ARRAY),
		MEMBER_INFO("pop",                                    var::VAR),
		MEMBER_INFO("clear",                                  var::_NULL),
		MEMBER_INFO("at",        {"index"}, {var::INT},       var::VAR),
		MEMBER_INFO("resize",    {"size"}, {var::INT},        var::_NULL),
		MEMBER_INFO("reserve",   {"size"}, {var::INT},        var::_NULL),
	};
	return member_info;
}

var Array::call_method(const String& p_method, const stdvec<var*>& p_args) {
	CHECK_METHOD_AND_ARGS();
	switch (p_method.const_hash()) {
		case "size"_hash:      return (int64_t)size();
		case "empty"_hash:     return empty();
		case "push_back"_hash: { push_back(*p_args[0]); return var(); }
		case "pop_back"_hash: { pop_back(); return var(); }
		case "append"_hash:    return append(*p_args[0]);
		case "pop"_hash:       return pop();
		case "clear"_hash: { clear(); return var(); }
		case "insert"_hash: { insert(*p_args[0], *p_args[1]); return var(); }
		case "at"_hash:        return at(p_args[0]->operator int64_t());
		case "resize"_hash: { resize(p_args[0]->operator int64_t()); return var(); }
		case "reserve"_hash: { reserve(p_args[0]->operator int64_t()); return var(); }
	}
	// TODO: add more.
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

Array::Array(const stdvec<var>& p_data) {
	_data = newptr<stdvec<var>>();
	for (const var& v : p_data) _data->push_back(v);
}

String Array::to_string() const {
	// TODO: if the array contains itself it'll stack overflow.
	std::stringstream ss;
	ss << "[ ";
	for (unsigned int i = 0; i < _data->size(); i++) {
		ss << _data->operator[](i).to_string();
		if (i != _data->size() - 1) ss << ", ";
		else ss << " ";
	}
	ss << "]";
	return ss.str();
}

var Array::pop() { var ret = this->operator[](size() - 1); pop_back(); return ret; }

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

Array& Array::operator=(const Array& p_other) {
	_data = p_other._data;
	return *this;
}

// Map  ----------------------------------------


MEMBER_INFO_IMPLEMENTATION(Map) {
	static const stdmap<String, const MemberInfo*> member_info = {
		MEMBER_INFO("size",                           var::INT),
		MEMBER_INFO("empty",                          var::BOOL),
		MEMBER_INFO("insert", {"what"}, {var::VAR},   var::_NULL),
		MEMBER_INFO("clear",                          var::_NULL),
		MEMBER_INFO("has",    {"what"}, {var::VAR},   var::BOOL),
	};
	return member_info;
}

var Map::call_method(const String& p_method, const stdvec<var*>& p_args) {
	CHECK_METHOD_AND_ARGS();
	switch (p_method.const_hash()) {
		case "size"_hash:   return (int64_t)size();
		case "empty"_hash:  return empty();
		case "insert"_hash: insert(*p_args[0], *p_args[1]); return var();
		case "clear"_hash:  clear(); return var();
		case "has"_hash:    return has(*p_args[0]);
	}
	// TODO: more.
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

Map::Map() {
	_data = newptr<_map_internal_t>();
}
Map::Map(const ptr<_map_internal_t>& p_data) {
	_data = p_data;
}
Map::Map(const Map& p_copy) {
	_data = p_copy._data;
}

String Map::to_string() const {
	std::stringstream ss;
	ss << "{ ";
	for (_map_internal_t::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (it != (*_data).begin()) ss << ", ";
		ss << it->second.key.to_string() << " : " << it->second.value.to_string();
	}
	ss << " }";
	return ss.str();
}

Map Map::copy(bool p_deep) const {
	Map ret;
	for (_map_internal_t::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (p_deep) {
			ret[(int64_t)it->first] = it->second.value.copy(true);
		} else {
			ret[(int64_t)it->first] = it->second.value;
		}
	}
	return ret;
}

// TODO: error message.
#define _INSERT_KEY_IF_HAVENT(m_key)                            \
	_map_internal_t::iterator it = (*_data).find(m_key.hash()); \
	if (it == _data->end()) (*_data)[m_key.hash()].key = m_key

var Map::operator[](const var& p_key) const { _INSERT_KEY_IF_HAVENT(p_key); return (*_data).operator[](p_key.hash()).value;  }
var& Map::operator[](const var& p_key) { _INSERT_KEY_IF_HAVENT(p_key); return (*_data).operator[](p_key.hash()).value; }
var Map::operator[](const char* p_key) const { _INSERT_KEY_IF_HAVENT(var(p_key)); return (*_data).operator[](var(p_key).hash()).value; }
var& Map::operator[](const char* p_key) { _INSERT_KEY_IF_HAVENT(var(p_key)); return (*_data).operator[](var(p_key).hash()).value; }

Map::_map_internal_t::iterator Map::begin() const { return (*_data).begin(); }
Map::_map_internal_t::iterator Map::end() const { return (*_data).end(); }
Map::_map_internal_t::iterator Map::find(const var& p_key) const { return (*_data).find(p_key.hash()); }

void Map::insert(const var& p_key, const var& p_value) {
	(*_data).insert(std::pair<size_t, _KeyValue>(p_key.hash(), _KeyValue(p_key, p_value)));
}
bool Map::has(const var& p_key) const { return find(p_key) != end(); }
void Map::clear() { _data->clear(); }

bool Map::operator ==(const Map& p_other) const {
	if (size() != p_other.size())
		return false;
	for (_map_internal_t::iterator it_other = p_other.begin(); it_other != p_other.end(); it_other++) {
		_map_internal_t::iterator it_self = find(it_other->second.key);
		if (it_self == end()) return false;
		if (it_self->second.value != it_other->second.value) return false;

	}
	return true;
}

Map& Map::operator=(const Map& p_other) {
	_data = p_other._data;
	return *this;
}

// Object -----------------------------------------------

var Object::operator+(const var& p_other) const { return __add(p_other); }
var Object::operator-(const var& p_other) const { return __sub(p_other); }
var Object::operator*(const var& p_other) const { return __mul(p_other); }
var Object::operator/(const var& p_other) const { return __div(p_other); }

var& Object::operator+=(const var& p_other) { return __add_eq(p_other); }
var& Object::operator-=(const var& p_other) { return __sub_eq(p_other); }
var& Object::operator*=(const var& p_other) { return __mul_eq(p_other); }
var& Object::operator/=(const var& p_other) { return __div_eq(p_other); }

var Object::operator[](const var& p_key) const { return __get_mapped(p_key); }
var& Object::operator[](const var& p_key) { THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "use __set_mapped() instead."); }

void Object::_bind_data(NativeClasses* p_native_classes) {
	BIND_METHOD("get_class_name", &Object::get_class_name);
	BIND_METHOD("get_parent_class_name", &Object::get_parent_class_name);
}
// call_method() should call it's parent if method not exists.
var Object::call_method(ptr<Object> p_self, const String& p_name, stdvec<var*>& p_args) {
	String class_name = p_self->get_class_name();
	String method_name = p_name;

	if (!NativeClasses::singleton()->is_class_registered(class_name)) {
		THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", class_name.c_str()));
	}

	ptr<BindData> bind_data = NativeClasses::singleton()->find_bind_data(class_name, p_name);

	if (bind_data) {
		if (bind_data->get_type() == BindData::METHOD) {
			return ptrcast<MethodBind>(bind_data)->call(p_self, p_args);

		} else if (bind_data->get_type() == BindData::STATIC_FUNC) {
			return ptrcast<StaticFuncBind>(bind_data)->call(p_args);

		} else {
			THROW_VARERROR(VarError::TYPE_ERROR,
				String::format("attribute named \"%s\" on type %s is not callable.", method_name.c_str(), p_self->get_class_name()));
		}
	} else {
		p_self->__call_method(p_name, p_args);
	}

	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no method named \"%s\".", p_self->get_class_name(), method_name.c_str()));
}

var Object::get_member(ptr<Object> p_self, const String& p_name) {
	String class_name = p_self->get_class_name();
	String member_name = p_name;

	if (!NativeClasses::singleton()->is_class_registered(class_name)) {
		//THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", class_name.c_str()));
		return p_self->__get_member(p_name);
	}

	ptr<BindData> bind_data = NativeClasses::singleton()->find_bind_data(class_name, member_name);
	if (bind_data) {
		if (bind_data->get_type() == BindData::MEMBER_VAR) {
			return ptrcast<PropertyBind>(bind_data)->get(p_self);
		} else if (bind_data->get_type() == BindData::STATIC_VAR) {
			return ptrcast<StaticPropertyBind>(bind_data)->get();
		} else if (bind_data->get_type() == BindData::STATIC_CONST) {
			return ptrcast<ConstantBind>(bind_data)->get();
		} else if (bind_data->get_type() == BindData::ENUM_VALUE) {
			return ptrcast<EnumValueBind>(bind_data)->get();
		} else if (bind_data->get_type() == BindData::ENUM) {
			return ptrcast<EnumBind>(bind_data)->get();

		} else {
			// TODO: function reference runtime object.
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("attribute named \"%s\" on type %s is not a property.", member_name.c_str(), p_self->get_class_name()));
		}
	} else {
		return p_self->__get_member(p_name);
	}
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no member named \"%s\"", p_self->get_class_name(), member_name.c_str()));
}


void Object::set_member(ptr<Object> p_self, const String& p_name, var& p_value) {
	String class_name = p_self->get_class_name();
	String member_name = p_name;

	if (!NativeClasses::singleton()->is_class_registered(class_name)) {
		//THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("the class \"%s\" isn't registered in native class entries.", class_name.c_str()));
		p_self->__set_member(p_name, p_value);
		return;
	}

	ptr<BindData> bind_data = NativeClasses::singleton()->find_bind_data(class_name, member_name);
	if (bind_data) {
		if (bind_data->get_type() == BindData::MEMBER_VAR) {
			ptrcast<PropertyBind>(bind_data)->get(p_self) = p_value;
		} else if (bind_data->get_type() == BindData::STATIC_VAR) {
			ptrcast<StaticPropertyBind>(bind_data)->get() = p_value;

		} else if (bind_data->get_type() == BindData::STATIC_CONST) {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't assign a value to constant named \"%s\" on type \"%s\".", member_name.c_str(), p_self->get_class_name()));
		} else if (bind_data->get_type() == BindData::ENUM_VALUE) {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't assign a value to enum value named \"%s\" on type \"%s\".", member_name.c_str(), p_self->get_class_name()));
		} else {
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("attribute named \"%s\" on type \"%s\" is not a property.", member_name.c_str(), p_self->get_class_name()));
		}
	} else {
		p_self->__set_member(p_name, p_value);
	}
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no member named \"%s\".", p_self->get_class_name(), member_name.c_str()));
}

const stdvec<const MemberInfo*> Object::get_member_info_list(const Object* p_instance) {
	if (p_instance) return NativeClasses::singleton()->get_member_info_list(p_instance->get_class_name());
	else return NativeClasses::singleton()->get_member_info_list(Object::get_class_name_s());
}
const MemberInfo* Object::get_member_info(const Object* p_instance, const String& p_member) {
	if (p_instance) return NativeClasses::singleton()->get_member_info(p_instance->get_class_name(), p_member);
	else return NativeClasses::singleton()->get_member_info(Object::get_class_name_s(), p_member);
}


// TODO: change these methods as static and call with an instance -> the base name could be found with self->get_class_name();

#define _OBJ_THROW_NOT_IMPL(m_name)\
	THROW_VARERROR(VarError::NOT_IMPLEMENTED, String("operator " #m_name " not implemented on base ") + get_class_name() + ".")

var Object::__call(stdvec<var*>& p_vars) { _OBJ_THROW_NOT_IMPL(__call()); }
var Object::__call_method(const String& p_method_name, stdvec<var*>& p_args) {
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no method named \"%s\".", get_class_name(), p_method_name.c_str()));
}
//var Object::operator()(stdvec<var>& p_vars) { return __call(p_vars); }

var  Object::__get_member(const String& p_member_name) {
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no member named \"%s\"", get_class_name(), p_member_name.c_str()));
}
void Object::__set_member(const String& p_member_name, var& p_value) {
	THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("type %s has no writable member named \"%s\".", get_class_name(), p_member_name.c_str()));
}

var Object::__get_mapped(const var& p_key) const { _OBJ_THROW_NOT_IMPL(__get_mapped()); }
void Object::__set_mapped(const var& p_key, const var& p_val) { _OBJ_THROW_NOT_IMPL(__set_mapped()); }
int64_t Object::__hash() const { _OBJ_THROW_NOT_IMPL(__hash()); }

var  Object::__iter_begin() { _OBJ_THROW_NOT_IMPL(__iter_begin()); }
bool Object::__iter_has_next() { _OBJ_THROW_NOT_IMPL(__iter_has_next()); }
var  Object::__iter_next() { _OBJ_THROW_NOT_IMPL(__iter_next()); }

var Object::__add(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__add()); }
var Object::__sub(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__sub()); }
var Object::__mul(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__mul()); }
var Object::__div(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__div()); }

var& Object::__add_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__add_eq()); }
var& Object::__sub_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__sub_eq()); }
var& Object::__mul_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__mul_eq()); }
var& Object::__div_eq(const var& p_other) { _OBJ_THROW_NOT_IMPL(__div_eq()); }

bool Object::__gt(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__gt()); }
bool Object::__lt(const var& p_other) const { _OBJ_THROW_NOT_IMPL(__lt()); }
bool Object::__eq(const var& p_other) const {
	if (p_other.get_type() != var::OBJECT) return false;
	return this == p_other.operator varh::ptr<varh::Object>().get();
}
#undef _OBJ_THROW_NOT_IMPL

// runtime types -------------------------------------

void _Iterator_String::_bind_data(NativeClasses* p_native_classes) {
	BIND_METHOD("__iter_has_next", &_Iterator_String::__iter_has_next);
	BIND_METHOD("__iter_next", &_Iterator_String::__iter_next);
}

void _Iterator_Array::_bind_data(NativeClasses* p_native_classes) {
	BIND_METHOD("__iter_has_next", &_Iterator_Array::__iter_has_next);
	BIND_METHOD("__iter_next", &_Iterator_Array::__iter_next);
}

void _Iterator_Map::_bind_data(NativeClasses* p_native_classes) {
	BIND_METHOD("__iter_has_next", &_Iterator_Map::__iter_has_next);
	BIND_METHOD("__iter_next", &_Iterator_Map::__iter_next);
}


// var -----------------------------------------------

String var::get_op_name_s(Operator op) {
	static const char* _names[] = {
		"OP_ASSIGNMENT",
		"OP_ADDITION",
		"OP_SUBTRACTION",
		"OP_MULTIPLICATION",
		"OP_DIVISION",
		"OP_MODULO",
		"OP_POSITIVE",
		"OP_NEGATIVE",
		"OP_EQ_CHECK",
		"OP_NOT_EQ_CHECK",
		"OP_LT",
		"OP_LTEQ",
		"OP_GT",
		"OP_GTEQ",
		"OP_AND",
		"OP_OR",
		"OP_NOT",
		"OP_BIT_LSHIFT",
		"OP_BIT_RSHIFT",
		"OP_BIT_AND",
		"OP_BIT_OR",
		"OP_BIT_XOR",
		"OP_BIT_NOT",
		"_OP_MAX_",
	};
	MISSED_ENUM_CHECK(Operator::_OP_MAX_, 23);
	return _names[op];
}

size_t var::hash() const {
	switch (type) {
		case _NULL:
			THROW_VARERROR(VarError::NULL_POINTER, "");
		case BOOL:   return std::hash<bool>{}(_data._bool);
		case INT:    return std::hash<int64_t>{}(_data._int);
		case FLOAT:  return std::hash<double>{}(_data._float);
		case STRING: return _data._string.hash();
		case ARRAY:
		case MAP:
			THROW_VARERROR(VarError::TYPE_ERROR, String::format("key of typt %s is unhashable.", get_type_name().c_str()));
		case OBJECT: DEBUG_BREAK(); // TODO: add hash method for objects?
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

bool var::is_hashable(var::Type p_type) {
	switch (p_type) {
		case _NULL:  return false;
		case BOOL:
		case INT:
		case FLOAT:
		case STRING:
			return true;
		case ARRAY:
		case MAP:
			return false;
		case OBJECT:
			return true;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

void var::clear() {
	clear_data();
	type = _NULL;
}

var var::copy(bool p_deep) const {
	switch (type) {
		case _NULL:
		case BOOL:
		case INT:
		case FLOAT:
		case STRING:
			return *this;
		case ARRAY: return _data._arr.copy(p_deep);
		case MAP: return _data._map.copy(p_deep);
		case OBJECT: return _data._obj->copy(p_deep);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_VARERROR(VarError::BUG, "can't reach here.");
}


/* constructors */
var::var() {
	_data._bool = false;
	type = _NULL;
}

var::var(const var& p_copy) {
	copy_data(p_copy);
	type = p_copy.type;
}

var::var(bool p_bool) {
	type = BOOL;
	_data._bool = p_bool;
}

var::var(int p_int) {
	type = INT;
	_data._int = p_int;
}

var::var(size_t p_int) {
	type = INT;
	_data._int = p_int;
}

var::var(int64_t p_int) {
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
	new(&_data._string) String(p_cstring);
}

var::var(const String& p_string) {
	type = STRING;
	new(&_data._string) String(p_string);
}

var::var(const Array& p_array) {
	type = ARRAY;
	new(&_data._arr) Array(p_array);
}

var::var(const Map& p_map) {
	type = MAP;
	new(&_data._map) Map(p_map);
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)                                                                      \
var var::operator m_op () {                                                                             \
	switch (type) {                                                                                     \
		case INT:  return m_op _data._int;                                                              \
		case FLOAT: return m_op _data._float;                                                           \
		default: THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED,                                        \
			String::format("operator " #m_op " not supported on base %s.", get_type_name().c_str()));   \
	}                                                                                                   \
	return var();                                                                                       \
}

#define VAR_OP_POST_INCR_DECR(m_op)                                                                     \
var var::operator m_op(int) {                                                                           \
	switch (type) {                                                                                     \
		case INT: return _data._int m_op;                                                               \
		case FLOAT: return _data._float m_op;                                                           \
		default: THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED,                                       \
			String::format("operator " #m_op " not supported on base %s.", get_type_name().c_str()));   \
	}                                                                                                   \
	return var();                                                                                       \
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

var var::operator[](const var& p_key) const {
	switch (type) {
		// strings can't return char as var&
		case STRING: return _data._string[p_key.operator int64_t()];
		case ARRAY:  return _data._arr[p_key.operator int64_t()];
		case MAP:    return _data._map[p_key];
		case OBJECT: return _data._obj->__get_mapped(p_key);
	}
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, String::format("operator[] not supported on base %s", get_type_name()));
}

var var::__get_mapped(const var& p_key) const {
	switch (type) {
		case STRING: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			int64_t index = p_key;
			return String(_data._string[index]);
		} break;
		case ARRAY: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			int64_t index = p_key;
			return _data._arr[index];
		} break;
		case MAP:
			if (!_data._map.has(p_key)) THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("key %s does not exists on base Map.", p_key.to_string()));
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->__get_mapped(p_key);
	}
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, String::format("operator[] not supported on base %s", get_type_name()));
}

void var::__set_mapped(const var& p_key, const var& p_value) {
	switch (type) {
		case STRING: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			if (p_value.get_type() != var::STRING) THROW_VARERROR(VarError::TYPE_ERROR, "expected a string value to assign");
			if (p_value.operator String().size() != 1) THROW_VARERROR(VarError::TYPE_ERROR, "expected a string of size 1 to assign");
			_data._string[p_key.operator int64_t()] = p_value.operator String()[0];
		} break;
		case ARRAY: {
			if (p_key.get_type() != var::INT) THROW_VARERROR(VarError::TYPE_ERROR, "expected a numeric value for indexing.");
			_data._arr[p_key.operator int64_t()] = p_value;
		} break;
		case MAP:
			_data._map[p_key] = p_value;
		case OBJECT:
			_data._obj->__set_mapped(p_key, p_value);
	}
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, String::format("operator[] not supported on base %s", get_type_name()));
}

var var::__iter_begin() {
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not iterable.");
		case var::INT:    THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "integer is not iterable.");
		case var::FLOAT:  THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "float is not iterable.");
		case var::STRING: return newptr<_Iterator_String>(&_data._string);
		case var::ARRAY:  return newptr<_Iterator_Array>(&_data._arr);
		case var::MAP:    return newptr<_Iterator_Map>(&_data._map);
		case var::OBJECT: return _data._obj.get()->__iter_begin();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var var::__call_internal(stdvec<var*>& p_args) {
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not callable.");
		case var::INT:    THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "integer is not callable.");
		case var::FLOAT:  THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "float is not callable.");
		case var::STRING: THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "String is not callable.");
		case var::ARRAY:  THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "Array is not callable.");
		case var::MAP:    THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED, "Map is not callable.");
		case var::OBJECT: return _data._obj.get()->__call(p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var var::call_method_internal(const String& p_method, stdvec<var*>& p_args) {

	// check var methods.
	switch (p_method.const_hash()) {
		case "to_string"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return to_string();
		case "copy"_hash:
			if (p_args.size() >= 2) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at maximum 1 argument.");
			if (p_args.size() == 0) return copy();
			return copy(p_args[0]->operator bool());
		case "hash"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return (int64_t)hash();
		case "get_type_name"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return get_type_name();

		// operators.
		case "__iter_begin"_hash:
			if (p_args.size() != 0) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 0 argument.");
			return __iter_begin();
		case "__get_mapped"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return __get_mapped(*p_args[0]);
		case "__set_mapped"_hash:
			if (p_args.size() != 2) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			__set_mapped(*p_args[0], *p_args[1]); return var();
		case "__add"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator +(*p_args[0]);
		case "__sub"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator -(*p_args[0]);
		case "__mul"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator *(*p_args[0]);
		case "__div"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator /(*p_args[0]);
		case "__add_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator +=(*p_args[0]);
		case "__sub_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator -=(*p_args[0]);
		case "__mul_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator *=(*p_args[0]);
		case "__div_eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator /=(*p_args[0]);
		case "__gt"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator >(*p_args[0]);
		case "__lt"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator <(*p_args[0]);
		case "__eq"_hash:
			if (p_args.size() != 1) THROW_VARERROR(VarError::INVALID_ARG_COUNT, "expected at exactly 1 argument.");
			return operator ==(*p_args[0]);

		case "__call"_hash:
			return __call_internal(p_args);
	}

	// type methods.
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("boolean has no attribute \"%s\".", p_method.c_str()));
		case var::INT:    THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("int has no attribute \"%s\".", p_method.c_str()));
		case var::FLOAT:  THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("float has no attribute \"%s\".", p_method.c_str()));
		case var::STRING: return _data._string.call_method(p_method, p_args);
		case var::ARRAY:  return _data._arr.call_method(p_method, p_args);
		case var::MAP:    return _data._map.call_method(p_method, p_args);
		case var::OBJECT: return Object::call_method(_data._obj, p_method, p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var var::get_member(const String& p_name) {
	switch (type) {
		case var::_NULL:   THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:    THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("boolean has no attribute \"%s\".", p_name.c_str()));
		case var::INT:     THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("int has no attribute \"%s\".", p_name.c_str()));
		case var::FLOAT:   THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("float has no attribute \"%s\".", p_name.c_str()));
		case var::STRING:
		case var::ARRAY:
		case var::MAP:
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" does not exists on base %s.", p_name.c_str(), get_type_name().c_str()));
		case var::OBJECT: return Object::get_member(_data._obj, p_name);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

void var::set_member(const String& p_name, var& p_value) {
	switch (type) {
		case var::_NULL:  THROW_VARERROR(VarError::NULL_POINTER, "");
		case var::BOOL:   THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("boolean has no attribute \"%s\".", p_name.c_str()));
		case var::INT:    THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("int has no attribute \"%s\".", p_name.c_str()));
		case var::FLOAT:  THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("float has no attribute \"%s\".", p_name.c_str()));
		case var::STRING:
		case var::ARRAY:
		case var::MAP:
			THROW_VARERROR(VarError::ATTRIBUTE_ERROR, String::format("attribute \"%s\" does not exists on \"%s\".", p_name.c_str(), get_type_name().c_str()));
		case var::OBJECT: Object::set_member(_data._obj, p_name, p_value);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

const MemberInfo* var::get_member_info(const String& p_name) const {
	if (type == OBJECT) return Object::get_member_info(_data._obj.get(), p_name);
	else return var::get_member_info_s(type, p_name);
}

const MemberInfo* var::get_member_info_s(var::Type p_type, const String& p_name) {
	switch (p_type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return nullptr;
		case var::STRING: return String::get_member_info(p_name);
		case var::ARRAY:  return Array::get_member_info(p_name);
		case var::MAP:    return Map::get_member_info(p_name);
		case var::OBJECT: return Object::get_member_info(nullptr, p_name);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

const stdvec<const MemberInfo*> var::get_member_info_list() const {
	if (type == OBJECT) return Object::get_member_info_list(_data._obj.get());
	else return get_member_info_list_s(type);
}

stdvec<const MemberInfo*> var::get_member_info_list_s(var::Type p_type) {
	stdvec<const MemberInfo*> ret;
	const stdmap<String, const MemberInfo*>* list = nullptr;
	switch (p_type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return ret;
		case var::STRING:
			list = &String::get_member_info_list();
			// [[fallthrough]]
		case var::ARRAY:
			list = &Array::get_member_info_list();
			// [[fallthrough]]
		case var::MAP:
			list = &Map::get_member_info_list();
			{
				stdmap<String, const MemberInfo*>::const_iterator it = list->begin();
				while (it != list->end()) {
					const MemberInfo* mi = (*it).second;
					ret.push_back(mi);
					it++;
				}
			}
			return ret;
		case var::OBJECT: {
			return Object::get_member_info_list(nullptr);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL:  return false;
		case BOOL:   return _data._bool;
		case INT:    return _data._int != 0;
		case FLOAT:  return _data._float != 0;
		case STRING: return _data._string.size() != 0;
		case ARRAY:  return !_data._arr.empty();
		case MAP:    return !_data._map.empty();
		case OBJECT: return _data._obj.operator bool();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"bool\".", get_type_name().c_str()));
}

var::operator int64_t() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		//case STRING: return  _data._string.to_int(); // throws std::exception
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"int\".", get_type_name().c_str()));
}

var::operator double() const {
	switch (type) {
		case BOOL: return (double)_data._bool;
		case INT: return (double)_data._int;
		case FLOAT: return _data._float;
		//case STRING: return  _data._string.to_float();
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"float\".", get_type_name().c_str()));
}

var::operator String() const {
	if (type != STRING)
		THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"String\".", get_type_name().c_str()));
	return _data._string;
}

String var::to_string() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return String(_data._int);
		case FLOAT: return String(_data._float);
		case STRING: return _data._string;
		case ARRAY: return _data._arr.to_string();
		case MAP: return _data._map.to_string();
		case OBJECT: return _data._obj->to_string();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	DEBUG_BREAK(); THROW_VARERROR(VarError::BUG, "can't reach here.");
}

var::operator Array() const {
	if (type == ARRAY) {
		return _data._arr;
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"Array\".", get_type_name().c_str()));
}

var::operator Map() const {
	if (type == MAP) {
		return _data._map;
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"Map\".", get_type_name().c_str()));
}

var::operator ptr<Object>() const {
	if (type == OBJECT) {
		return _data._obj;
	}
	THROW_VARERROR(VarError::TYPE_ERROR, String::format("can't cast \"%s\" to \"Object\".", get_type_name().c_str()));
}

/* operator overloading */
		/* comparison */

#define THROW_OPERATOR_NOT_SUPPORTED(m_op)                                                         \
do {                                                                                               \
	THROW_VARERROR(VarError::OPERATOR_NOT_SUPPORTED,                                                \
		String::format("operator \"" STR(m_op) "\" not supported on operands \"%s\" and \"%s\".",  \
			get_type_name().c_str(), p_other.get_type_name().c_str())                              \
	);                                                                                             \
} while(false)

bool var::operator==(const var& p_other) const {

	// TODO: if other.type == object and it has overload operator.

	switch (type) {
		case _NULL: return false;
		case BOOL:   {
			switch (p_other.type) {
				case BOOL:  return _data._bool == p_other._data._bool;
				case INT:   return _data._bool == (bool) p_other._data._int;
				case FLOAT: return _data._bool == (bool) p_other._data._float;
			}
		} break;
		case INT:    {
			switch (p_other.type) {
				case BOOL:  return (bool) _data._int == p_other._data._bool;
				case INT:   return _data._int        == p_other._data._int;
				case FLOAT: return _data._int        == p_other._data._float;
			}
		} break;
		case FLOAT:  {
			switch (p_other.type) {
				case BOOL:  return (bool)_data._float == p_other._data._bool;
				case INT:   return _data._float       == p_other._data._int;
				case FLOAT: return _data._float       == p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string == p_other._data._string;
			break;
		}
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr == p_other.operator Array();
			}
			break;
		}
		case MAP: {
			if (p_other.type == MAP) {
				return _data._map == p_other.operator Map();
			}
			break;
		}
		case OBJECT: {
			return _data._obj->__eq(p_other);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	return false; // different types.
}

bool var::operator!=(const var& p_other) const {
	return !operator==(p_other);
}

bool var::operator<(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int)    _data._bool < (int)p_other._data._bool;
				case INT:   return (int)    _data._bool < p_other._data._int;
				case FLOAT: return (double) _data._bool < p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int < (int)p_other._data._bool;
				case INT:   return _data._int < p_other._data._int;
				case FLOAT: return _data._int < p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float < (double) p_other._data._bool;
				case INT:   return _data._float < p_other._data._int;
				case FLOAT: return _data._float < p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() < *p_other.operator Array().get_data();
			break;
		}
		case MAP: {
			break;
		}
		case OBJECT: {
			return _data._obj->__lt(p_other);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(<);
}

bool var::operator>(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int)    _data._bool > (int) p_other._data._bool;
				case INT:   return (int)    _data._bool > p_other._data._int;
				case FLOAT: return (double) _data._bool > p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int > (int) p_other._data._bool;
				case INT:   return _data._int > p_other._data._int;
				case FLOAT: return _data._int > p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float > (int)p_other._data._bool;
				case INT:   return _data._float > p_other._data._int;
				case FLOAT: return _data._float > p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() > *p_other.operator Array().get_data();
			break;
		}
		case MAP:
			break;
		case OBJECT: {
			return _data._obj->__gt(p_other);
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(>);
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
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t) _data._bool + (int64_t) p_other._data._bool;
				case INT:   return (int64_t) _data._bool + p_other._data._int;
				case FLOAT: return (double) _data._bool  + p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int          + (int64_t) p_other._data._bool;
				case INT:   return _data._int          + p_other._data._int;
				case FLOAT: return (double) _data._int + p_other._data._float;
			}

		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float + (double) p_other._data._bool;
				case INT:   return _data._float + (double) p_other._data._int;
				case FLOAT: return _data._float + p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string + p_other._data._string;
			break;
		}
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr + p_other._data._arr;
			}
			break;
		}
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__add(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(+);
}

var var::operator-(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t) _data._bool - (int64_t) p_other._data._bool;
				case INT:   return (int64_t) _data._bool - p_other._data._int;
				case FLOAT: return (double) _data._bool  - p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int         - (int64_t)p_other._data._bool;
				case INT:   return _data._int         - p_other._data._int;
				case FLOAT: return (double)_data._int - p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float - (double)p_other._data._bool;
				case INT:   return _data._float - (double)p_other._data._int;
				case FLOAT: return _data._float - p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__sub(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(-);
}

var var::operator *(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t)_data._bool * (int64_t)p_other._data._bool;
				case INT:   return (int64_t)_data._bool * p_other._data._int;
				case FLOAT: return (double)_data._bool  * p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int         * (int64_t)p_other._data._bool;
				case INT:   return _data._int         * p_other._data._int;
				case FLOAT: return (double)_data._int * p_other._data._float;
				case STRING: {
					String ret;
					for (int64_t i = 0; i < _data._int; i++) {
						ret += p_other._data._string;
					}
					return ret;
				}
				case ARRAY: {
					Array ret;
					ret._data->reserve(ret._data->size() * _data._int);
					for (int64_t i = 0; i < _data._int; i++) {
						ret._data->insert(ret._data->end(), p_other._data._arr.begin(), p_other._data._arr.end());
					}
					return ret;
					break;
				}
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float * (double)p_other._data._bool;
				case INT:   return _data._float * (double)p_other._data._int;
				case FLOAT: return _data._float * p_other._data._float;
			}
		} break;
		case STRING:
			if (p_other.type == INT) {
				String ret;
				for (int64_t i = 0; i < p_other._data._int; i++) {
					ret += _data._string;
				}
				return ret;
			}
			break;
		case ARRAY:
			if (p_other.type == INT) {
				Array ret;
				ret._data->reserve(_data._arr.size() * p_other._data._int);
				for (int64_t i = 0; i < p_other._data._int; i++) {
					ret._data->insert(ret._data->end(), _data._arr.begin(), _data._arr.end());
				}
				return ret;
			}
			break;
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(*);
}

var var::operator /(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (double)_data._bool  / p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._int         / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._int         / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return (double)_data._int / p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					return _data._float / p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__div(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(/);
}

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
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(%);
}

var& var::operator+=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    + (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    + p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool + p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   =         _data._int + (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int + p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int + p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float + (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float + (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float + p_other._data._float;         return *this;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING) {
				_data._string += p_other._data._string;
				return *this;
			}
			break;
		}
		case ARRAY: {
			if (p_other.type == ARRAY) {
				_data._arr += p_other._data._arr;
				return *this;
			}
			break;
		}
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__add_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(+=);
}

var& var::operator-=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    - (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    - p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool - p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int)   _data._int - (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int - p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int - p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float - (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float - (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float - p_other._data._float;         return *this;
			}
		} break;
		case STRING:
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__sub_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(-=);
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int64_t)_data._bool    * (int64_t)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int64_t)_data._bool    * p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool     * p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int64_t)_data._int * (int64_t)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int  * p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int  * p_other._data._float;      return *this;
				case STRING: {
					String self;
					for (int64_t i = 0; i < _data._int; i++) {
						self += p_other._data._string;
					}
					type = STRING; new(&_data._string) String(self); return *this;
				}
				case ARRAY: {
					Array self;
					for (int64_t i = 0; i < _data._int; i++) {
						self._data->insert(self.end(), p_other._data._arr.begin(), p_other._data._arr.end());
					}
					clear();
					type = ARRAY; _data._arr = self; return *this;
				}
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float * (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float * (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float * p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			if (p_other.type == INT) {
				String self;
				for (int64_t i = 0; i < p_other._data._int; i++) {
					self += _data._string;
				}
				_data._string = self; return *this;
			}
			break;
		case ARRAY: {
			if (p_other.type == INT) {
				_data._arr.reserve(_data._arr.size() * p_other._data._int);
				for (int64_t i = 0; i < p_other._data._int -1; i++) {
					_data._arr._data->insert(_data._arr.end(), _data._arr.begin(), _data._arr.end());
				}
				return *this;
			}
		}
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(*=);
}

var& var::operator/=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / (int)p_other._data._bool; return *this;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / p_other._data._int;       return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._bool / p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._int   = (int)   _data._int / (int)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._int   =         _data._int / p_other._data._int;        return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._int / p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._int;   return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) THROW_VARERROR(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			break;
		case ARRAY:
		case MAP:
			break;
		case OBJECT:
			return _data._obj->__mul_eq(p_other);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
	THROW_OPERATOR_NOT_SUPPORTED(/=);
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
	THROW_OPERATOR_NOT_SUPPORTED(%=);
}

void var::copy_data(const var& p_other) {
	clear_data();
	type = p_other.type;
	switch (p_other.type) {
		case var::_NULL: break;
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
			new(&_data._string) String(p_other._data._string);
			break;
		case var::ARRAY:
			new(&_data._arr) Array(p_other._data._arr);
			break;
		case var::MAP:
			new(&_data._map) Map(p_other._data._map);
			break;
		case var::OBJECT:
			new(&_data._obj) ptr<Object>(p_other._data._obj);
			return;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
}

void var::clear_data() {
	switch (type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
			return;
		case var::STRING:
			_data._string.~String();
			return;
		case var::ARRAY:
			_data._arr.~Array();
			break;
		case var::MAP:
			_data._map.~Map();
			break;
		case var::OBJECT:
			_data._obj = nullptr;
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 9);
}

String var::get_type_name() const {
	if (type == OBJECT) {
		return _data._obj->get_class_name();
	} else {
		return get_type_name_s(type);
	}
}

}


#if defined(UNDEF_VAR_DEFINES)
#undef func
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_BREAK
#undef DEBUG_PRINT
#undef VAR_ASSERT
#undef MISSED_ENUM_CHECK
#undef UNDEF_VAR_DEFINES
#endif
