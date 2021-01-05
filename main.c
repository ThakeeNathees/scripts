/* 
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "var.h"

#include <stdio.h>
#include <assert.h>

/**
 * Main is here used to run \ref var tests
 */
int main() {

	// basic encoding and decoding of values
	assert(AS_BOOL(VALUE_BOOL(false)) == false);
	assert(AS_INTEGER(VALUE_INTEGER(-42)) == -42);
	assert(AS_NUMBER(VALUE_NUMBER(3.14)) == 3.14);

	assert(IS_BOOL(VALUE_BOOL(22)));
	assert(IS_INTEGER(VALUE_INTEGER(-3.14)));
	assert(IS_NUMBER(VALUE_NUMBER(99999.99999)));

	return 0;
}
