
#include "_var.h"

#define D_VEC(m_vect, m_dim, m_t) STR_CAT3(m_vect, m_dim, m_t)

var var::tmp;

std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.operator std::string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr) {
	p_ostream << p_arr.operator std::string();
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
Array::operator std::string() const {
	std::stringstream ss;
	ss << "[ ";
	for (unsigned int i = 0; i < _data->size(); i++) {
		ss << _data->operator[](i).operator std::string();
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
			ret.push_back(operator[](i).copy());
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
// var -----------------------------------------------

void var::clear() {
	type = _NULL;
}

var var::copy(bool p_deep) const {
	switch (type) {
		case _NULL:
		case BOOL:
		case INT:
		case FLOAT:
		case STD_STRING:
		case VECT2F:
		case VECT2I:
		case VECT3F:
		case VECT3I:
			return *this;
		case ARRAY: return _data_arr.copy(p_deep);
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
	type = STD_STRING;
	_data_std_string = std::string(p_cstring);
}

var::var(const std::string& p_std_string) {
	type = STD_STRING;
	_data_std_string = std::string(p_std_string);
}

#define VAR_VECT_CONSTRUCTOR(m_dim, m_t, m_T)        \
var::var(const D_VEC(Vect, m_dim, m_t)& p_vect) {    \
	type = D_VEC(VECT, m_dim, m_T);                  \
	memcpy(_data._mem, &p_vect, sizeof(_data._mem)); \
}
VAR_VECT_CONSTRUCTOR(2, f, F)
VAR_VECT_CONSTRUCTOR(2, i, I)
VAR_VECT_CONSTRUCTOR(3, f, F)
VAR_VECT_CONSTRUCTOR(3, i, I)
#undef VAR_VECT_CONSTRUCTOR


var::var(const Array& p_array) {
	type = ARRAY;
	_data_arr = p_array._data;
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

var& var::operator[](size_t index) {
	switch (type) {
		case ARRAY:
			return _data_arr[index];
		// case DICTIONARY:
	}
	VAR_ERR("invalid operator[]");
	return var::tmp;
}
var& var::operator[](size_t index) const {
	switch (type) {
		case ARRAY:
			return _data_arr[index];
			// case DICTIONARY:
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
		case STD_STRING: return _data_std_string.size() != 0;

		case VECT2F: return *DATA_PTR_CONST(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR_CONST(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case ARRAY: return _data_arr.empty();
		case OBJ_PTR: return _data_obj._ptr == nullptr;
	}
	VAR_ERR("invalid casting");
	return false;
}

var::operator int() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		case STD_STRING: return  std::stoi(_data_std_string);
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator double() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return _data._float;
		case STD_STRING: return  std::stod(_data_std_string);
		default: VAR_ERR("invalid casting");
	}
	return -1;
}

var::operator std::string() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return std::to_string(_data._int);
		case FLOAT: return std::to_string(_data._float);
		case STD_STRING: return _data_std_string;
		case VECT2F: return *DATA_PTR_CONST(Vect2f);
		case VECT2I: return *DATA_PTR_CONST(Vect2i);
		case VECT3F: return *DATA_PTR_CONST(Vect3f);
		case VECT3I: return *DATA_PTR_CONST(Vect3i);
		case ARRAY: return _data_arr.operator std::string();
		case OBJ_PTR: return std::string("Object(").append(_data_obj.name).append(")");
	}
	VAR_ERR("invalid casting");
	return "TODO";
}

var::operator const char* () const {
	return operator std::string().c_str();
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
		case ARRAY: return _data_arr;
		default: VAR_ERR("invalid casting");
	}
	return Array();
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
		case STD_STRING: {
			if (p_other.type == STD_STRING)
				return _data_std_string == p_other._data_std_string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, ==) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, ==) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, ==) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, ==) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data_arr == p_other.operator Array();
			}
			break;
		}
		case OBJ_PTR: {
			if (p_other.type == OBJ_PTR)
				return _data_obj._ptr == p_other._data_obj._ptr;
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
		case STD_STRING: {
			if (p_other.type == STD_STRING)
				return _data_std_string < p_other._data_std_string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, < ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, < ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, < ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, < ) }
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data_arr.get_data() < *p_other.operator Array().get_data();
			break;
		}
		case OBJ_PTR: {
			if (p_other.type == OBJ_PTR)
				return _data_obj._ptr < p_other._data_obj._ptr;
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
		case STD_STRING: {
			if (p_other.type == STD_STRING)
				return _data_std_string < p_other._data_std_string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, > ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, > ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, > ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, > ) }
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data_arr.get_data() > *p_other.operator Array().get_data();
			break;
		}
		case OBJ_PTR: {
			if (p_other.type == OBJ_PTR)
				return _data_obj._ptr > p_other._data_obj._ptr;
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
		case STD_STRING: {
			if (p_other.type == STD_STRING)
				return _data_std_string + p_other._data_std_string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, + ) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, + ) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, + ) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, + ) }
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data_arr + p_other._data_arr;
			}
			break;
		}
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
		case STD_STRING:
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
		case STD_STRING:
			break; // TODO: maybe python like
		case VECT2F: { VAR_SWITCH_VECT(2, f, *) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, *) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, *) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, *) }
		case ARRAY:
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
		case STD_STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, /) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, /) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, /) }
		case ARRAY:
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
		case STD_STRING: {
			if (p_other.type == STD_STRING) {
				_data_std_string += p_other._data_std_string;
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
				_data_arr += p_other._data_arr;
				return *this;
			}
			break;
		}
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
		case STD_STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, -=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, -=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, -=) }
		case ARRAY:
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
		case STD_STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, *=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, *=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, *=) }
		case ARRAY:
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
		case STD_STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /=) }
		case VECT2I: { VAR_SWITCH_VECT(2, i, /=) }
		case VECT3F: { VAR_SWITCH_VECT(3, f, /=) }
		case VECT3I: { VAR_SWITCH_VECT(3, i, /=) }
		case ARRAY:
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

#undef VAR_CASE_OP
#undef VAR_SWITCH_PRIME_TYPES
#undef VAR_SWITCH_VECT
#undef CASE_DIV_DATA
#undef SWITCH_DIV_TYPES