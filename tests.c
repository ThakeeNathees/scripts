/* 
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "var.h"
#include "clogger.h"

#include <stdio.h>
#include <assert.h>

#define TEST_ASSERT(expr)                                                         \
  do {													                          \
    if (!(expr)) {										                          \
		failed = true;															  \
		cafe_cLogfError("[TEST_FAILED]: \"%s\" was false at line: %i\n",	      \
                             #expr, __LINE__);                                    \
	}													                          \
  } while (false)

/**
 * Main is here used to run \ref var tests
 */
int main() {

	bool failed = false;

	// basic encoding and decoding of values
	TEST_ASSERT(AS_BOOL(VAR_BOOL(false)) == false);
	TEST_ASSERT(AS_INT(VAR_INT(-42)) == -42);
	TEST_ASSERT(AS_NUM(VAR_NUM(3.14)) == 3.14);

	TEST_ASSERT(IS_BOOL(VAR_BOOL(22)));
	TEST_ASSERT(IS_INT(VAR_INT(-3.14)));
	TEST_ASSERT(IS_NUM(VAR_NUM(99999.99999)));

	{
		var pi = VAR_NUM(3.14);
		var ans = VAR_INT(42);
		double sum = AS_NUM(pi) + AS_INT(ans);
		TEST_ASSERT(sum == (3.14 + 42));
	}

	if (!failed) cafe_cLogfSuccess("ALL TESTS PASSED\n");
	getchar();

	return 0;
}
