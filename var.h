/*
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef VAR_H
#define VAR_H

/** @file
 * A simple single header dynamic type system library for small dynamic typed
 * languages using a technique called NaN-tagging (optional). The method is 
 * inspired from the wren (https://wren.io/) an awsome language written by the
 * author of "Crafting Interpreters" Bob Nystrom and it's contrbuters. 
 * Reference:
 *     https://github.com/wren-lang/wren/blob/main/src/vm/wren_value.h
 *     https://leonardschuetz.ch/blog/nan-boxing/
 * 
 * The previous implementation was to add a type field to every \ref var
 * and use smart pointers(C++17) to object with custom destructors,
 * which makes the programme in effect for small types such null, bool,
 * int and float.
 */

/** __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS are a workaround to
 * allow C++ programs to use stdint.h macros specified in the C99
 * standard that aren't in the C++ standard
 */
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <stdbool.h>
#include <string.h>

/** To use dynamic variably-sized struct with a tail array add an array at the
 * end of the struct with size \ref DYNAMIC_TAIL_ARRAY. This method was a
 * legacy standard called "struct hack".
*/
#if __STDC_VERSION__ >= 199901L
  /** for std >= c99  it's just `arr[]` */
  #define DYNAMIC_TAIL_ARRAY
#else
  #define DYNAMIC_TAIL_ARRAY 0
#endif

/** Nan-Tagging could be disable for debugging/portability purposes. 
 * To disable define `VAR_NAN_TAGGING 0`, otherwise it defaults to Nan-Tagging
 */
#ifndef VAR_NAN_TAGGING
  #define VAR_NAN_TAGGING 1
#endif

/**
 * The IEEE 754 double precision float bit representation.
 *
 * 1 Sign bit
 * | 11 Exponent bits
 * | |          52 Mantissa (i.e. fraction values) bits
 * | |          |
 * S[Exponent-][Mantissa------------------------------------------]
 *
 * if all bits of the exponent are set it's a NaN ("Not a Number") value.
 *
 *  v~~~~~~~~~~ NaN value
 * -11111111111----------------------------------------------------
 * 
 * We define a our variant \ref var as an unsigned 64 bit integer (we treat it
 * like a bit array) if the exponent bits were not set, just reinterprit it as
 * a IEEE 754 double precision 64 bit number. Other wise we there are a lot of
 * different combination of bits we can use for our custom tagging, this method
 * is called NaN-Tagging.
 *
 * There are two kinds of NaN values "signalling" and "quiet". The first one is
 * intended to halt the execution but the second one is to continue the
 * execution quietly. We get the quiet NaN by setting the highest mentissa bit.
 *
 *             v~Highest mestissa bit
 * -[NaN      ]1---------------------------------------------------
 * 
 * if sign bit set, it's a heap allocated pointer.
 * |            these 3 bits are type tags representing 8 different types
 * |            vvv
 * S[NaN      ]1---------------------------------------------------
 *                 ^~~~~~~~ 48 bits to represent the value (51 for pointer)
 * 
 * On a 32-bit machine a pointer size is 32 and on a 64-bit machine actually 48
 * bits are used for pointers. Ta-da, now we have double precision number,
 * primitives, pointers all inside a 64 bit sequence and for numbers it doesn't
 * require any bit mask operations, which means math on the var is now even
 * faster. (Once again thanks https://wren.io/ for this amazing reference)
 *
 */

#if VAR_NAN_TAGGING

/**
 * Custom 3 bits type tagging
 * 000 : UNDEFINED
 * 001 : NULL
 * 010 : FALSE
 * 011 : TRUE
 * 100 : INTEGER
 * 101 : ???
 * 111 : ???
 */

/** masks and payloads */
#define MASK_SIGN ((uint64_t)0x8000000000000000)
#define MASK_QNAN ((uint64_t)0x7ffc000000000000)
#define MASK_TYPE ((uint64_t)0x0007000000000000)

#define MASK_INTEGER (MASK_QNAN | (uint64_t)0x0004000000000000)
#define MASK_OBJECT  (MASK_QNAN | (uint64_t)0x8000000000000000)

#define PAYLOAD_INTEGER ((uint64_t)0x00000000ffffffff)
#define PAYLOAD_OBJECT  ((uint64_t)0x0000ffffffffffff)

/** primitive types */
#define VALUE_UNDEFINED (MASK_QNAN | (uint64_t)0x0000000000000000)
#define VALUE_NULL      (MASK_QNAN | (uint64_t)0x0001000000000000)
#define VALUE_FALSE     (MASK_QNAN | (uint64_t)0x0002000000000000)
#define VALUE_TRUE      (MASK_QNAN | (uint64_t)0x0003000000000000)

/** encode types */
#define VALUE_BOOL(value)    ((value)? VALUE_TRUE : VALUE_FALSE)
#define VALUE_INTEGER(value) (MASK_INTEGER | (uint32_t)(int32_t)(value))
#define VALUE_NUMBER(value)  (var_double_to_var(value))
#define VALUE_OBJECT(value)  ((var)(MASK_OBJECT | (uint64_t)(uintptr_t)(value)))
#define VALUE_STRING(value)  VALUE_OBJECT(value)

/** check types */
#define IS_UNDEFINED(value) ((value) == VALUE_UNDEFINED)
#define IS_NULL(value)      ((value) == VALUE_NULL)
#define IS_FALSE(value)     ((value) == VALUE_FALSE)
#define IS_TRUE(value)      ((value) == VALUE_TRUE)
#define IS_BOOL(value)      (IS_TRUE(value) || IS_FALSE(value))
#define IS_INTEGER(value)   ((value & MASK_INTEGER) == MASK_INTEGER)
#define IS_NUMBER(value)    ((value & MASK_QNAN) != MASK_QNAN)
#define IS_OBJECT(value)    ((value & MASK_OBJECT) == MASK_OBJECT)
// TODO: IS_STRING(), ...

/** decode types */
#define AS_BOOL(value)    ((value) == VALUE_TRUE)
#define AS_INTEGER(value) ((int32_t)((value) & PAYLOAD_INTEGER))
#define AS_NUMBER(value)  (var_var_to_double(value))
#define AS_OBJECT(value)  ((var_Object*)(value & PAYLOAD_OBJECT))
// TODO: AS_STRING(), ...

#else
// TODO:
#endif // VAR_NAN_TAGGING

typedef enum /* Object_Type */ {
	OBJ_STRING,
	OBJ_ARRAY,
	OBJ_MAP,

	OBJ_USER,
} var_Object_Type;

typedef struct /* Object */ {
	var_Object_Type type;
} var_Object;

typedef struct /* String < Object */ {
	var_Object* _super;

	uint32_t length;    /**< Length of the string in \ref data */
	uint32_t capacity;  /**< Size of allocated \ref data */
	char data[DYNAMIC_TAIL_ARRAY];
} var_String;

#if VAR_NAN_TAGGING

typedef uint64_t var;

#else

typedef enum /* var_Type */ {
	VAR_UNDEFINED, /**< Internal type for exceptions */
	VAR_NULL,      /**< Null pointer type */
	VAR_BOOL,      /**< Yin and yang of software */
	VAR_INT,       /**< Only 32bit integers (to consistance with Nan-Tagging) */
	VAR_FLOAT,     /**< Floats are stored as (64bit) double */

	VAR_STRING,    /**< Strings are also \ref var_Object type */
	VAR_OBJECT,    /**< Base type for all \ref var_Object types */
} var_Type;

typedef struct /* var */ {
	var_Type type;
	union {
		bool _bool;
		int _int;
		double _float;
		var_Object* _obj;
	};
} var;

#endif // VAR_NAN_TAGGING

#if VAR_NAN_TAGGING
/** A union to reinterpret a double as raw bits and back. */
typedef union /* var_DoubleBits */ {
	uint64_t bits64;
	uint32_t bits32[2];
	double num;
} var_DoubleBits;
#endif

static inline var var_double_to_var(double value) {
#if VAR_NAN_TAGGING
	var_DoubleBits bits;
	bits.num = value;
	return bits.bits64;
#else
	// TODO:
#endif // VAR_NAN_TAGGING
}

static inline double var_var_to_double(var value) {
#if VAR_NAN_TAGGING
	var_DoubleBits bits;
	bits.bits64 = value;
	return bits.num;
#else
	// TODO:
#endif // VAR_NAN_TAGGING
}

/** The `var_` prefix in the types are for namespace. Defining 
 * USING_NAMESPACE_VAR will remote all `var_` prefix from the types.
*/
#ifdef USING_NAMESPACE_VAR
  #define _REMOVE_VAR_PREFIX(type) typedef var_##type type
    _REMOVE_VAR_PREFIX(String);
    _REMOVE_VAR_PREFIX(Object);
    _REMOVE_VAR_PREFIX(Object_Type);
    _REMOVE_VAR_PREFIX(DoubleBits);

    _REMOVE_VAR_PREFIX(var_to_double);
    _REMOVE_VAR_PREFIX(double_to_var);
  #undef _REMOVE_VAR_PREFIX
#endif // USING_NAMESPACE_VAR

#endif // VAR_H