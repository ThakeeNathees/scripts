
#include "_var.h"

#define D_VEC(m_vect, m_dim, m_t) STR_CAT3(m_vect, m_dim, m_t)

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
	_data_arr = p_array._data_arr;
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

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL: return false;
		case BOOL: return _data._bool;
		case INT: return _data._int != 0;
		case FLOAT: return _data._float != 0;
		case STD_STRING: return _data_std_string.size() != 0;

		case VECT2F: return *DATA_PTR(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR(Vect3f) == Vect3f();
		case ARRAY: return _data_arr.empty();
		case OBJ_PTR: return _data_obj._ptr == nullptr;
			break; // TODO:

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
		case VECT2F: return *DATA_PTR(Vect2f);
		case VECT2I: return *DATA_PTR(Vect2i);
		case VECT3F: return *DATA_PTR(Vect3f);
		case VECT3I: return *DATA_PTR(Vect3i);
		case ARRAY:
			break;
		case OBJ_PTR: return std::string("Object(").append(_data_obj.name).append(")");
	}
	VAR_ERR("invalid casting");
	return "TODO";
}


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
#undef VAR_VECT_CAST

var::operator Array() const {
	switch (type) {
		case ARRAY: return Array(_data_arr);
		default: VAR_ERR("invalid casting");
	}
	return Array();
}

