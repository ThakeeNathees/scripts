/* 
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "var.h"
#define CLOGGER_IMPLEMENT
#include "clogger.h"

#include <stdio.h>
#include <assert.h>

#define TEST_ASSERT(expr)                                                         \
  do {													                          \
    if (!(expr)) {										                          \
		tests_failed = true;												      \
		clogger_logfError("[TEST_FAILED]: \"%s\" was false at line: %i\n",	      \
                             #expr, __LINE__);                                    \
	}													                          \
  } while (false)

#ifdef PLATFORM_WINDOWS
  #define sleep Sleep
#else
  #error "define sleep here for dummy progress"
#endif

enum CafeColors {
	_COL_DONTUSE_ = 0,
	COL_WHITE = 1,
	COL_GREEN = 2,
	COL_YELLOW = 3,
	COL_RED = 4,

	COL_BROWN,
	COL_ORANGE,
};

/**
 * Main is here used to run \ref var tests
 */
int main() {

	// set cafe color palletes
	clogger_ColorPalette cafe_pallete;
	cafe_pallete.colors[COL_WHITE] = clogger_ColorRGB(180, 170, 150);
	cafe_pallete.colors[COL_GREEN] = clogger_ColorRGB(130, 160, 100);
	cafe_pallete.colors[COL_YELLOW] = clogger_ColorRGB(180, 200, 70);
	cafe_pallete.colors[COL_RED] = clogger_ColorRGB(200, 70, 90);
	cafe_pallete.colors[COL_BROWN] = clogger_ColorRGB(160, 120, 50);
	cafe_pallete.colors[COL_ORANGE] = clogger_ColorRGB(200, 160, 60);
	clogger_setColorPalette(cafe_pallete);
	clogger_init();

	clogger_log("==== Cafe color logger tests: ===========\n", COL_WHITE, false);
	clogger_log("  some long message here? ", COL_WHITE, false);
	clogger_log("more here ", COL_BROWN, false);
	clogger_log("[y/n]\n", COL_ORANGE, false);
	clogger_log("  DummySuccess: Compiled successfully!\n", COL_GREEN, false);
	clogger_log("  DummyWarning: Ooh that's deprecated!\n", COL_YELLOW, false);
	clogger_log("  DummyError  : Oops something went wrong!\n", COL_RED, false);
	for (int i = 0; i <= 30; i++) {
		clogger_progress("  dummy progress...", i, 30);
		sleep(10);
	} printf("\n");
	clogger_log("=========================================\n", COL_WHITE, false);
	printf("\n");

	bool tests_failed = false;

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

		pi = ADD_CONST(pi);
		TEST_ASSERT(IS_CONST(pi));
		pi = REMOVE_CONST(pi);
		TEST_ASSERT(!IS_CONST(pi));
	}
	if (!tests_failed) clogger_logfSuccess("ALL TESTS PASSED\n");

	if (_isatty(_fileno(stdout)))
		getchar(); // pause.

	return 0;
}
