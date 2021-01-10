/*
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef clogger_H
#define clogger_H

/** Cafe single header color logger library
 *
 * // define imlpementation only a single *.c source file
 * #define CLOGGER_IMPLEMENT
 * #include "clogger.h"
 *
 * You should call `clogger_init();` before any of your calling logging calls.
 * You can define your own pallete with `clogger_ColorPalette` and apply it
 * from `clogger_setColorPalette(your_pallete)` function. There is a list of 
 * public API functions declared. `clogger_iColor` is just a 8bit unsigned
 * integer value which between 0 and `PALLETE_MAX_SIZE`, for default values
 * use CLOGGER_COL_WHITE, CLOGGER_COL_GREEN, ... If you can use your own values
 * if you've defined your own pallete. For examples see the implementation in
 * `test.c`
*/

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

/** Default color needed for basic logging
*/
#define CLOGGER_COL_DONTUSE 0
#define CLOGGER_COL_WHITE   1
#define CLOGGER_COL_GREEN   2
#define CLOGGER_COL_YELLOW  3
#define CLOGGER_COL_RED     4

/** supported max 16 different colors. First four colors must be
 * white, green, yellow, red (for logging error, warning, etc.)
*/
#define PALLETE_MAX_SIZE 16

#define CLOGGER_PROGRESS_BAR 30

 /** color logger public API */
typedef struct clogger_Color clogger_Color;
typedef struct clogger_ColorPalette clogger_ColorPalette;
typedef uint8_t clogger_iColor;

clogger_Color clogger_ColorRGB(uint8_t r, uint8_t g, uint8_t b);

clogger_ColorPalette clogger_getDefaultPallete();
void clogger_setColorPalette(clogger_ColorPalette pallate);
void clogger_init();

void clogger_logf(clogger_iColor color, bool _stderr, const char* fmt, ...);
void clogger_logfVA(const char* fmt, va_list args, bool _stderr,
	clogger_iColor color);
void clogger_log(const char* msg, clogger_iColor color, bool _stderr);

void clogger_logfSuccess(const char* fmt, ...);
void clogger_logfWarning(const char* fmt, ...);
void clogger_logfError(const char* fmt, ...);

void clogger_progress(const char* msg, int done, int total);

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
struct clogger_Color {
	uint8_t r, g, b;
};

struct clogger_ColorPalette  {
	clogger_Color colors[PALLETE_MAX_SIZE];
};


#endif // clogger_H

#ifdef CLOGGER_IMPLEMENT

/** The default color palette of cprint (global)
*/
clogger_ColorPalette* g_clogger_color_pallete;

//void clogger_progress(const char* msg, int done, int total)

void clogger_init() {
	if (g_clogger_color_pallete == NULL) {
		clogger_setColorPalette(clogger_getDefaultPallete());
	}
}

clogger_Color clogger_ColorRGB(uint8_t r, uint8_t g, uint8_t b) {
	clogger_Color ret = {r, g, b};
	return ret;
}

void clogger_logf(clogger_iColor color, bool _stderr, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	clogger_logfVA(fmt, args, _stderr, color);
	va_end(args);
}

void clogger_logfSuccess(const char* p_fmt, ...) {
	va_list args;
	va_start(args, p_fmt);
	clogger_logfVA(p_fmt, args, false, CLOGGER_COL_GREEN);
	va_end(args);
}

void clogger_logfWarning(const char* p_fmt, ...) {
	va_list args;
	va_start(args, p_fmt);
	clogger_logfVA(p_fmt, args, true, CLOGGER_COL_YELLOW);
	va_end(args);
}

void clogger_logfError(const char* p_fmt, ...) {
	va_list args;
	va_start(args, p_fmt);
	clogger_logfVA(p_fmt, args, true, CLOGGER_COL_RED);
	va_end(args);
}

void clogger_logfVA(const char* fmt, va_list args, bool _stderr, clogger_iColor color) {

	char buf[VSNPRINTF_BUFF_SIZE + 1]; // +1 for the terminating character
	int len = vsnprintf(buf, VSNPRINTF_BUFF_SIZE, fmt, args);

	if (len <= 0) return;
	// Output is too big, will be truncated
	if ((unsigned int)len >= VSNPRINTF_BUFF_SIZE) len = VSNPRINTF_BUFF_SIZE;
	buf[len] = 0;
	clogger_log((const char*)buf, color, _stderr);
}

clogger_ColorPalette clogger_getDefaultPallete() {
	static clogger_ColorPalette s_pallete = { {
		/* _COL_DONTUSE_ */ { 0, 0, 0 },

		/* COL_WHITE  */ { 255, 255, 255 },
		/* COL_GREEN  */ {   0, 255,   0 },
		/* COL_YELLOW */ { 255, 255,   0 },
		/* COL_RED    */ { 255,   0,   0 },
		
		/* _COL_MAX_  */
	}};
	return s_pallete;
}

/** for other terminal emulator which support ANSI (git base, mysys, putty, ...)
*/
void cclogger_logANSI(const char* message, clogger_iColor color, bool _stderr) {
	// \033[38;2;R;G;Bm msg \033[0;00m
	assert(g_clogger_color_pallete != NULL && "did you forgot to call clogger_init()");

	clogger_Color col = g_clogger_color_pallete->colors[color];

	char buff_color[23]; int ptr = 0;
	ptr += sprintf(buff_color + ptr, "%s", "\033[38;2;");
	ptr += sprintf(buff_color + ptr, "%i", col.r);
	ptr += sprintf(buff_color + ptr, "%s", ";");
	ptr += sprintf(buff_color + ptr, "%i", col.g);
	ptr += sprintf(buff_color + ptr, "%s", ";");
	ptr += sprintf(buff_color + ptr, "%i", col.b);
	ptr += sprintf(buff_color + ptr, "%s", "m");
	buff_color[22] = '\0';
	fprintf((_stderr) ? stderr : stdout, "%s%s%s", buff_color, message, "\033[0;00m");
}

void clogger_progress(const char* msg, int done, int total) {
	float precentage = (float)done / (float)total;
	clogger_logf(CLOGGER_COL_WHITE, false, "\r%s [", msg);
	int i = 0;
	for (; i < precentage * CLOGGER_PROGRESS_BAR; i++)
		clogger_log("#", CLOGGER_COL_GREEN, false);
	for (; i < CLOGGER_PROGRESS_BAR; i++)
		clogger_log(" ", CLOGGER_COL_WHITE, false);
	clogger_logf(CLOGGER_COL_WHITE, false, "] %i%%", (int)(precentage * 100));
	fflush(stdout);

}

/** PLATFORM DEPENDENT CODE **************************************************/

#ifdef PLATFORM_WINDOWS

#ifndef NOMINMAX /**< mingw already has defined for us. */
#define NOMINMAX
#endif

#include <io.h>  /**< for isatty() */
#include <Windows.h>
#undef ERROR /**< This will polute symbol `ERROR` later */

#include <direct.h>

void clogger_setColorPalette(clogger_ColorPalette pallete) {
	static clogger_ColorPalette s_pallete;
	s_pallete = pallete;
	g_clogger_color_pallete = &s_pallete;

	if (_isatty(_fileno(stdout))) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFOEX info;
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	
		GetConsoleScreenBufferInfoEx(hConsole, &info);
		for (int i = 1; i < PALLETE_MAX_SIZE; i++) {
			uint8_t r = g_clogger_color_pallete->colors[i].r;
			uint8_t g = g_clogger_color_pallete->colors[i].g;
			uint8_t b = g_clogger_color_pallete->colors[i].b;
			info.ColorTable[i] = RGB(r, g, b);
		}
		SetConsoleScreenBufferInfoEx(hConsole, &info);
	}
	// else we use ANSI color codes
}

static void _win32_setConsoleColor(clogger_iColor color) {
	assert(g_clogger_color_pallete != NULL && "did you forgot to call clogger_init()");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void clogger_log(const char* message, clogger_iColor color, bool _stderr) {
	FILE* out = (_stderr) ? stderr : stdout;
	if (_isatty(_fileno(out))) {
		_win32_setConsoleColor(color);
		fprintf(out, "%s", message);
		_win32_setConsoleColor(CLOGGER_COL_WHITE);
	} else {
		cclogger_logANSI(message, color, _stderr);
		fflush(out);
	}
}

#elif defined(PLATFORM_LINUX)

#error "TODO:"

#else

#error "TODO:"

#endif // PLATFORM_WINDOWS

#endif // CLOGGER_IMPLEMENT