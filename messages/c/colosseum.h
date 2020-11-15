#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

typedef int8_t tag_t;
typedef int32_t msize_t;

msize_t mrecv_size = 0;

msize_t msend(int f, void const *data, msize_t size, tag_t tag);
msize_t msend_str(int f, char const *data, tag_t tag);
msize_t msend_int(int f, int32_t data, tag_t tag);
msize_t mrecv(int f, void *data, msize_t size, tag_t *tag);
tag_t mrecv2(int f, void *data, msize_t size);

inline msize_t msend(int f, void const *data, msize_t size, tag_t tag) {
	// TODO: if machine is big endian convert bytes from big to little endian
	write(f, &size, sizeof(size));
	write(f, &tag, sizeof(tag));
	return size > 0 ? write(f, data, size) : 0;
}

inline msize_t msend_str(int f, char const *data, tag_t tag) {
	msize_t size = strlen(data);
	return msend(f, data, size, tag);
}

inline msize_t msend_int(int f, int32_t data, tag_t tag) {
	return msend(f, &data, sizeof(data), tag);
}

inline msize_t mrecv(int f, void *data, msize_t size, tag_t *tag) {
	// TODO: if machine is big endian convert bytes from little to big endian
	msize_t message_size;
	read(f, &message_size, sizeof(message_size));
	if (message_size > size) {
		fprintf(stderr, "colosseum error: incoming message size (%d) exceeded maximum buffer size (%d)\n", message_size, size);
		exit(1);
	}
	read(f, tag, sizeof(*tag));
	return size > 0 ? read(f, data, message_size) : 0;
}

inline tag_t mrecv2(int f, void *data, msize_t size) {
	tag_t tag;
	mrecv_size = mrecv(f, data, size, &tag);
	return tag;
}

