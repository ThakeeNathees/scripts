/*
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef CPRINT_H
#define CPRINT_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

 /** color logger public API */

typedef enum cprint_Color cprint_Color;

void cafe_setCursorPos(int line, int column);   /**< set console cursor pos */
void cafe_getCursorPos(int* line, int* column); /**< get console cursor pos */

void cafe_cLogf(cprint_Color forground, cprint_Color background,
	bool _stderr, const char* fmt, ...);
void cafe_logf(const char* fmt, ...);
void cafe_cLogfVA(const char* fmt, va_list args, bool _stderr,
	cprint_Color forground, cprint_Color background);
void cafe_cLog(const char* msg, bool _stderr, cprint_Color forground,
	cprint_Color background);

void cafe_cLogfSuccess(const char* fmt, ...);
void cafe_cLogfWarning(const char* fmt, ...);
void cafe_cLogfError(const char* fmt, ...);

/** Define our own platform macro
 */
#ifndef _PLATFORM_DEFINED_
  #define _PLATFORM_DEFINED_
  #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define PLATFORM_WINDOWS
  #elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
      #define PLATFORM_IOS_SIMULATOR
    #elif TARGET_OS_IPHONE
      #define PLATFORM_IOS
    #elif TARGET_OS_MAC
      #define PLATFORM_APPLE
    #else
      #error "Unknown Apple platform"
    #endif
  #elif defined(__linux__)
    #define PLATFORM_LINUX
  #elif defined(__unix__)
    #define PLATFORM_UNIX
  #elif defined(_POSIX_VERSION)
    #define PLATFORM_POSIX
  #else
    #error "Unknown Platform"
  #endif
#endif // _PLATFORM_DEFINED_

/** The buffer size for vnsprintf(...)
 */
#ifndef VSNPRINTF_BUFF_SIZE
#define VSNPRINTF_BUFF_SIZE 8192
#endif

/** The platform independant color values
 */
enum cprint_Color {
	DEFAULT = -1,
	BLACK = 0,

	L_BLUE = 1,
	L_GREEN = 2,
	L_SKYBLUE = 3,
	L_RED = 4,
	L_PINK = 5,
	L_YELLOW = 6,
	L_WHITE = 7,
	L_GRAY = 8,

	D_BLUE = 9,
	D_GREEN = 10,
	D_SKYBLUE = 11,
	D_RED = 12,
	D_PINK = 13,
	D_YELLOW = 14,
	D_WHITE = 15,

};

#endif // CPRINT_H

#ifdef PLATFORM_WINDOWS

#ifndef NOMINMAX /**< mingw already has defined for us. */
#define NOMINMAX
#endif

#include <Windows.h>
#undef ERROR /**< This will polute symbol `ERROR` later */

#include <direct.h>

static void _win32_setConsoleColor(cprint_Color forground,
	cprint_Color background) {
	if (background == DEFAULT)
		background = BLACK;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		(int)background << 4 | (int)forground);
}

void cafe_getCursorPos(int* line, int* column) {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	COORD pos;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi)) {
		pos = cbsi.dwCursorPosition;
	} else {
		// TODO: error here.
	}
	*line = pos.X;
	*column = pos.Y;
}

void cafe_setCursorPos(int line, int column) {
	COORD coord;
	coord.X = line;
	coord.Y = column;

	if (!SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord)) {
		// TODO: error here.
	}
}

void cafe_cLogf(cprint_Color forground, cprint_Color background,
	bool _stderr, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	cafe_cLogfVA(fmt, args, _stderr, forground, background);
	va_end(args);
}

void cafe_cLogfVA(const char* fmt, va_list args, bool _stderr,
	cprint_Color forground, cprint_Color background) {
	if (forground == DEFAULT)
		forground = L_WHITE;
	if (background == DEFAULT)
		background = BLACK;
	char buf[VSNPRINTF_BUFF_SIZE + 1]; // +1 for the terminating character
	int len = vsnprintf(buf, VSNPRINTF_BUFF_SIZE, fmt, args);

	if (len <= 0) return;
	// Output is too big, will be truncated
	if ((unsigned int)len >= VSNPRINTF_BUFF_SIZE) len = VSNPRINTF_BUFF_SIZE;
	buf[len] = 0;

	_win32_setConsoleColor(forground, background);
	if (_stderr) fprintf(stderr, "%s", buf);
	else fprintf(stdout, "%s", buf);
	_win32_setConsoleColor(L_WHITE, DEFAULT);
}

void cafe_cLog(const char* message, bool _stderr, cprint_Color forground,
	cprint_Color background) {
	if (forground == DEFAULT)
		forground = L_WHITE;
	if (background == DEFAULT)
		background = BLACK;
	_win32_setConsoleColor(forground, background);
	if (_stderr) {
		fprintf(stderr, "%s", message);
	} else {
		fprintf(stdout, "%s", message);
	}
	_win32_setConsoleColor(L_WHITE, DEFAULT);
}

void cafe_cLogfSuccess(const char* p_fmt, ...) {
	va_list args;
	va_start(args, p_fmt);
	cafe_cLogfVA(p_fmt, args, false, L_GREEN, DEFAULT);
	va_end(args);
}

void cafe_cLogfWarning(const char* p_fmt, ...) {
	va_list args;
	va_start(args, p_fmt);
	cafe_cLogfVA(p_fmt, args, true, D_YELLOW, DEFAULT);
	va_end(args);
}

void cafe_cLogfError(const char* p_fmt, ...) {
	va_list args;
	va_start(args, p_fmt);
	cafe_cLogfVA(p_fmt, args, true, D_RED, DEFAULT);
	va_end(args);
}

#elif defined(PLATFORM_LINUX)

#error "TODO:"

#else

#error "TODO:"

#endif // PLATFORM_WINDOWS
