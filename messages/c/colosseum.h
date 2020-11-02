#pragma once

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

typedef int8_t tag_t;
typedef int32_t msize_t;

msize_t mrecv_size = 0;

inline msize_t msend(int f, void const *data, msize_t size, tag_t tag) {
	write(f, &size, sizeof(size));
	write(f, &tag, sizeof(tag));
	return size > 0 ? write(f, data, size) : 0;
}

inline msize_t mrecv(int f, void *data, tag_t *tag) {
	msize_t size;
	read(f, &size, sizeof(size));
	read(f, tag, sizeof(*tag));
	return size > 0 ? read(f, data, size) : 0;
}

inline tag_t mrecv2(int f, void *data) {
	tag_t tag;
	mrecv_size = mrecv(f, data, &tag);
	return tag;
}

inline msize_t msend_str(int f, char const *data, tag_t tag) {
	msize_t size = strlen(data);
	return msend(f, data, size, tag);
}

inline msize_t msend_int(int f, int32_t data, tag_t tag) {
	return msend(f, &data, sizeof(data), tag);
}

