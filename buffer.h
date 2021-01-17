/*
 *  Copyright (c) 2020-2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <memory.h>
#include <errno.h>

/** The factor by which the buffer will grow when it's capacity reached */
#define GROW_FACTOR 2

/** The initial capacity of the buffer */
#define MIN_CAPACITY 16

/** If the user proveds a custom realloc function use it (probably for tracing)
 * otherwise use buffer_realloc. The realloc function expected to be match the
 * symbol : `void* realloc_fn(void* oldptr, size_t size, void* useData);` see
 * buffer_realloc for details.
 */
#ifndef REALLOC_FN
	#define REALLOC_FN buffer_realloc
#endif

/** If the size is zero the pointer will freed, if the pointer is null it'll
 * malloc() and return the given size. userData is reserved to future memory 
 * tracing. if the size != 0 and any failure occure it'll reutrn NULL otherwise
 * returns the pointer to allocated memory. */
void* buffer_realloc(void* oldptr, size_t size, void* userData);

typedef struct buffer buffer;
struct buffer {
	size_t count;
	size_t capacity;
	void* data;
};

/** Initialize a new buffer int instance */
void buffer_init(buffer* self);

/** Ensure the minimum capacity to [count]. Since buffer is using void* to, it
 * requires the element size to be allocated, and it's using realloc function
 * it requires a user data for tracing, gc etc. */
void buffer_reserve(buffer* self, size_t count, size_t sizeofElem, void* userData);

/** clear the buffer and set the data to NULL */
void buffer_clear(buffer* self, void* userData);

#ifdef BUFFER_IMPLEMENT

void* buffer_realloc(void* oldptr, size_t size, void* useData) {
	if (size == 0) {
		free(oldptr);
		return NULL;
	}

	return realloc(oldptr, size);
}

void buffer_init(buffer* self) {
	self->count = 0;
	self->capacity = 0;
	self->data = NULL;
}

void buffer_reserve(buffer* self, size_t count, size_t sizeofElem, void* userData) {
	// TODO: make the capacity a power of 2
	if (self->capacity < count) {
		REALLOC_FN(self->data, count * sizeofElem, userData);
	}
}

void buffer_clear(buffer* self, void* userData) {
	REALLOC_FN(self->data, 0, userData);
}

#endif // BUFFER_IMPLEMENT

#endif // BUFFER_H