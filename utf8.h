/*
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef UTF8_H
#define UTF8_H

/** @file
 * A tiny UTF-8 utility library.
 * 
 * 
 * Utf-8 is an elegant character encoding which I just love it's simplicity,
 * and compatibility It's just a wonderful hack of all time. A single byte 
 * length utf-8 character is the same as an ASCII character. In case if you
 * don't know about ASCII encoding it's just how a character is represented in
 * a single byte. For an example the character 'A' is 01000001, 'B' is 01000010
 * and so on. The first bit in is always 0 called parity bit, it's a way to 
 * check if  some of the bits have flipped by noice back in the old age of
 * computers. Parity bit should be equal to the sum of the rest of the bits mod
 * 2. So we have 7 bits to represent ASCII which is 127 different characters.
 * But utf-8 can potentially encode 2,164,864 characters.
 * 
 * The length of a utf-8 character would vary from 1 to 4. If it's a single
 * byte character, it's starts with a 0 and rest of the 7 bytes have the 
 * value. It's not just like ASCII, it is ASCII (compatable). For the 2 bytes
 * character the first byte starts with 110....., for the 3 bytes character
 * it's starts with 1110.... and for the 4 byte it's 11110... The first byte
 * is called the leading byte and the rest of the bytes of the character is
 * called continuation bytes.
 * 
 * <pre>
 * example: 
 *                  v-- leading byte   v-- continuation byte => 2 bytes
 *             é =  11000011           10101001
 *                  ^^^                ^^
 *                  110 means 2 bytes  10 means continuation
 * 
 * (note that the character é is 8 bit long with ANSI encoding)
 * </pre>
 * 
 * USAGE:
 *     // define imlpementation only a single *.c source file like this
 *     #define UTF8_IMPLEMENT
 *     #include "utf8.h"
 *     // in MSVC #pragma execution_character_set("utf-8") if for utf8 literals
*/

#include <stdint.h>

/** Returns the number of bytes the the [value] would take to encode. returns 0
 * if the value is invalid utf8 representation.
 *
 * <pre>
 * For single byte character, represented as 0xxxxxxx
 * the payload is 7 bytes so the maximum value would be 0x7f
 *
 * For 2 bytes characters, represented as 110xxxxx 10xxxxxx
 * the payload is 11 bits               | xxx xxxx xxxx |
 * so the maximum value would be 0x7ff  |  7   f    f   |
 *
 * For 3 bytes character, represented as 1110xxxx 10xxxxxx 10xxxxxx
 * the payload is 16 bits               | xxxx xxxx xxxx xxxx |
 * so the maximum value would be 0xffff | f    f    f    f    |
 *
 * For 4 bytes character, represented as 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * the payload is 21 bits                     | x xxxx xxxx xxxx xxxx xxxx |
 * so the maximum value *SHOULD* be 0x1fffff  | 1 f    f    f    f    f    |
 * but in RFC3629 §3 (https://tools.ietf.org/html/rfc3629#section-3) UTF-8 is
 * limited to 0x10FFFF to match the limits of UTF-16.
 * </pre>
 */
int utf8_encodeBytesCount(int value);

/** Returns the number of bytes the the leading [byte] contains. returns 1 if
 * the byte is an invalid utf8 leading byte (to skip pass to the next byte). */
int utf8_decodeBytesCount(uint8_t byte);

/** Encodes the 32 bit value into a byte array which should be a size of 4 and
 * returns the number of bytes the value encoded (if invalid returns 0, that
 * how many it write to the buffer */
int utf8_encodeValue(int value, uint8_t* bytes);

/** Decodes from the leading [byte] and write the value to param [value] and
 * returns the number of bytes the value decoded, if invalid write -1 to the
 * value */
int utf8_decodeBytes(uint8_t* bytes, int* value);


#endif // UTF8_H

#ifdef UTF8_IMPLEMENT

#define B1(first) 0b##first
#define B2(first, last) 0b##first##last
#define B3(first, second, last) 0b##first##second##last
#define B4(first, second, third, last) 0b##first##second##third##last

int utf8_encodeBytesCount(int value) {
	if (value <= 0x7f) return 1;
	if (value <= 0x7ff) return 2;
	if (value <= 0xffff) return 3;
	if (value <= 0x10ffff) return 4;
	
	// if we're here means it's an invalid leading byte
	return 0;
}

int utf8_decodeBytesCount(uint8_t byte) {

	if ((byte >> 7) == 0b0) return 1;
	if ((byte >> 6) == 0b10) return 1; //< continuation byte
	if ((byte >> 5) == 0b110) return 2;
	if ((byte >> 4) == 0b1110) return 3;
	if ((byte >> 3) == 0b11110) return 4;

	// if we're here means it's an invalid utf8 byte
	return 1;
}

int utf8_encodeValue(int value, uint8_t* bytes) {

	if (value <= 0x7f) {
		*bytes = value & 0x7f;
		return 1;
	}

	// 2 byte character 110xxxxx 10xxxxxx -> last 6 bits write to 2nd byte and
	// first 5 bit write to first byte
	if (value <= 0x7ff) {
		*(bytes++) = B2(110, 00000) | ((value & B2(11111, 000000)) >> 6);
		*(bytes)   = B2(10, 000000) | ((value & B1(111111)));
		return 2;
	}

	// 3 byte character 1110xxxx 10xxxxxx 10xxxxxx -> from last, 6 bits write
	// to  3rd byte, next 6 bits write to 2nd byte, and 4 bits to first byte.
	if (value <= 0xffff) {
		*(bytes++) = B2(1110, 0000) | ((value & B3(1111, 000000, 000000)) >> 12);
		*(bytes++) = B2(10, 000000) | ((value & B2(111111, 000000)) >> 6);
		*(bytes)   = B2(10, 000000) | ((value & B1(111111)));
		return 3;
	}

	// 4 byte character 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx -> last 6 bits to
	// to 4th byte, next 6 bits to 3rd byte, next 6 bits to 2nd byte, 3 bits
	// first byte.
	if (value <= 0x10ffff) {
		*(bytes++) = B2(11110, 000) | ((value & B4(111, 000000, 000000, 000000)) >> 18);
		*(bytes++) = B2(10, 000000) | ((value & B3(111111, 000000, 000000)) >> 12);
		*(bytes++) = B2(10, 000000) | ((value & B2(111111, 000000)) >> 6);
		*(bytes)   = B2(10, 000000) | ((value & B1(111111)));
		return 4;
	}

	return 0;
}

int utf8_decodeBytes(uint8_t* bytes, int* value) {

	int continue_bytes = 0;
	int byte_count = 1;
	int _value = 0;

	if ((*bytes & B2(11, 000000)) == B2(10, 000000)) {
		*value = *bytes;
		return byte_count;
	}

	else if ((*bytes & B2(111, 00000)) == B2(110, 00000)) {
		continue_bytes = 1;
		_value = (*bytes & B1(11111));
	}

	else if ((*bytes & B2(1111, 0000)) == B2(1110, 0000)) {
		continue_bytes = 2;
		_value = (*bytes & B1(1111));
	}

	else if ((*bytes & B2(11111, 000)) == B2(11110, 000)) {
		continue_bytes = 3;
		_value = (*bytes & B1(111));
	}

	else {
		// Invalid leading byte
		return -1;
	}
	
	// now add the continuation bytes to the _value
	while (continue_bytes--) {
		bytes++, byte_count++;

		if ((*bytes & B2(11, 000000)) != B2(10, 000000)) return -1;

		_value = (_value << 6) | (*bytes & B2(00, 111111));
	}

	*value = _value;
	return byte_count;
}

#undef B1
#undef B2
#undef B3
#undef B4
#endif // UTF8_IMPLEMENT