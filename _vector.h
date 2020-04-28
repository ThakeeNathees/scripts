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

#ifndef VECTOR_H
#define VECTOR_H

#include "core.h"

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
	template<typename T2>
	Vect2(const T2& p_copy) : x((T)p_copy.x), y((T)p_copy.y) {}

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
#define M_OP_TEMPLATE(T2)                    \
	Vect2<T>& operator*=(T2 p_val) const {   \
		x *= p_val; y*= p_val;               \
		return *this;                        \
	}
	M_OP_TEMPLATE(float)
	M_OP_TEMPLATE(int)
#undef M_OP_TEMPLATE

	Vect2<T>& operator/=(const Vect2<T>& p_other) {
		if (p_other.x == 0 || p_other.y == 0)
			VAR_ERR("zero division error");
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
	operator const char* () const {
		return operator std::string().c_str();
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
	template <typename T2>
	Vect3<T> operator*(T2 p_val) const {
		return Vect3<T>(x * p_val, y * p_val, z * p_val);
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
	operator std::string() const { // FIXME
		return std::string("Vect3(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(", ")
			.append(std::to_string(z)).append(")");
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect2<T>& p_vect) {
	p_ostream << p_vect.operator std::string();
	return p_ostream;
}
template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect3<T>& p_vect) {
	p_ostream << p_vect.operator std::string();
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