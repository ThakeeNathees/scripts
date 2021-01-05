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
 * author of "Crafting Interpreters" Bob Nystrom and it's contrbuters. This
 * header library written using the wren's value header
 * (https://github.com/wren-lang/wren/blob/main/src/vm/wren_value.h) as a
 * reference
 * 
 * The previous implementation was to add a type field to every \ref var
 * and use smart pointers(c++17) to object with custom destructors,
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
 * legacy standard called "struct hack". (Don't forget to store the size in 
 * the sutrct).
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
  #define VAR_NAN_TAGGING 0
#endif

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

/** The `var_` prefix in the types are for namespace. Defining 
 * USING_NAMESPACE_VAR will remote all `var_` prefix from the types.
*/
#ifdef USING_NAMESPACE_VAR
  #define _REMOVE_VAR_PREFIX(type) typedef var_##type type
    _REMOVE_VAR_PREFIX(String);
    _REMOVE_VAR_PREFIX(Object);
    _REMOVE_VAR_PREFIX(Object_Type);
  #undef _REMOVE_VAR_PREFIX
#endif // USING_NAMESPACE_VAR

#endif // VAR_H