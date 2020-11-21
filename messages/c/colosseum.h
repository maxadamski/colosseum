#pragma once

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#define DEBUG

typedef int8_t   Tag;

//msize_t mrecv_size = 0;

typedef enum {
	T_U8   = 0x00, T_U32  = 0x01, T_I8   = 0x02, T_I32 = 0x03,
	T_F32  = 0x04, T_F64  = 0x05, T_BOOL = 0x06,
	T_ARR  = 0x10,
} Arg_Type;

extern u32 const type_size[];

extern Arg_Type const format_type[];

extern u32 const format_size[];

i32 msendf(int f, Tag tag, char const *fmt, ...);

void mscanf(u8 *data, char const *fmt, ...);

/** Low-level send (without splitting arguments), returns bytes written
 *
 *
 */
i32 msend(int f, Tag tag, void const *data, u32 size);


/** Low-level function to receive any message
 *  @param tag - incoming message tag
 *  @param buffer - incoming message payload
 *  @param size - maximum buffer size
 *  @returns buffer size
 */
i32 mrecv(int f, Tag *tag, void *data, u32 size);

