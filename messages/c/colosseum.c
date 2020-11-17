#include "colosseum.h"

Arg_Type const format_type[] = {
	['u']=T_U8 , ['U']=T_U32, ['i']=T_I8, ['I']=T_I32, 
	['f']=T_F32, ['F']=T_F64, ['b']=T_BOOL
};

u32 const format_size[] = {
	['u']=1, ['U']=4, ['i']=1, ['I']=4,
	['f']=4, ['F']=8, ['b']=1
};

u32 const type_size[] = {
	[T_U8]=1, [T_U32]=4, [T_I8]=1, [T_I32]=4,
	[T_F32]=4, [T_F64]=8, [T_BOOL]=1,
};

i32 msendf(int f, Tag tag, char const *fmt, ...) {
	u32 const max_arg = 8;
	u32 const max_dim = 8;
	u32 const max_types = (4*max_dim+1)*max_arg;
	u32 const max_header = 6 + max_types;
	u8 header[max_header];

	u8 *types = header + 10;
	u32 n_types = 0;

	void *values[max_arg];
	u32 sizes[max_arg];
	u32 data_size = 0;

	va_list args;
	va_start(args, fmt);

	int arg = 0;
	while (*fmt) {
		if (*fmt++ != '%') {
			continue;
		}

		if (arg >= max_arg) panic("argument limit exceeded (%d)\n", arg);
		values[arg] = va_arg(args, void*);

		Arg_Type base = format_type[(u8) *fmt++];
		sizes[arg] = type_size[base];

		if (*fmt++ == '[') {
			u32 dims[8];
			u8 n_dims = 0;
			while (*fmt != ']') {
				if (*fmt == '%') {
					dims[n_dims++] = va_arg(args, u32);
					fmt++;

				} else {
					u32 dim = 0;
					while ('0' <= *fmt && *fmt <= '9')
						dim = dim * 10 + *fmt++ - '0';
					if (dim == 0) panic("array dimensions must be greater than 0\n");
					dims[n_dims++] = dim;
				}
				if (*fmt == ']') break;
				if (*fmt != ',') panic("expected `,` but got `%c`\n", *fmt);
				fmt++;
			}
			fmt++;

			if (n_dims == 0) panic("array must have at least one dimension\n");
			types[n_types++] = 0x80 | ((0x07 & n_dims) << 4) | (base & 0x0F);
			for (int i = 0; i < n_dims; i++) {
				u32 dim = dims[i];
				sizes[arg] *= dim;
				*((u32*)(types + n_types)) = dim;
				n_types += 4;
			}
		} else {
			types[n_types++] = base & 0x0F;
		}

		data_size += sizes[arg];
		arg++;
	}

	debug("sending header (%u bytes)\n", 10 + n_types);

	((u32*)(header+0))[0] = n_types + data_size;
	header[4] = tag;
	header[5] = arg;
	((u32*)(header+6))[0] = n_types;

	debug("[head] ");
	for (int i = 0; i < 10; i++) debug("%02X ", header[i]);
	debug("\n");

	u32 sent = write(f, &header, 10);

	debug("[type] ");
	for (int i = 0; i < n_types; i++) debug("%02X ", types[i]);
	debug("\n");

	sent += write(f, types, n_types);

	for (int i = 0; i < arg; i++) {
		u8 *value = (u8*) (values + i);
		debug("[arg%d] ", i);
		for (int j = 0; j < sizes[i]; j++) debug("%02X ", value[j]);
		debug("\n");
		sent += write(f, value, sizes[i]);
	}

	va_end(args);

	return sent;
}

i32 mrecvf(int f, Tag tag, char const *fmt, ...) {
	// TODO: wait for message with tag `tag`
	i32 recv = 0;
	u8 head[10];
	u32 type_i = 0;
	recv += read(f,head, 10);

	u32 cur_size = 0;
	u32 max_size = *((u32*)(head + 0));
	tag          = *((u8*)(head + 4));
	u8  max_arg  = *((u8*)(head + 5));
	u32 max_type = *((u32*)(head + 6));

	u8 arg = 0;

	u8 type[max_type];
	recv += read(f, &type, max_type);

	debug("recv head ");
	for (int i = 0; i < 10; i++) debug("%02X ", head[i]);
	debug("\n");
	debug("recv type ");
	for (int i = 0; i < max_type; i++) debug("%02X ", type[i]);
	debug("\n");

	va_list args;
	va_start(args, fmt);

	while (type_i < max_type && arg < max_arg) {
		u8 typ  = type[type_i++];
		u8 is_a = (typ & 0x80) >> 7;
		u8 dims = (typ & 0x70) >> 4;
		u8 base = typ & 0x0F;
		u32 size = type_size[base];
		for (u8 i = 0; is_a && i < dims; i++) {
			size *= *((u32*)(type + type_i));
			type_i += 4;
		}
		
		if (cur_size + size > max_size) panic("message body size exceeded maximum\n");
		void *argp = va_arg(args, void*);
		recv += read(f, argp, size);

		debug("recv arg%d ", arg);
		for (int i = 0; i < size; i++) {
			if (i > 16) {
				debug("... (size=%u)", size);
				break;
			}
			debug("%02X ", ((u8*)argp)[i]);
		}
		debug("\n");

		arg++;
	}

	va_end(args);
	return 0;
}

i32 msend(int f, Tag tag, void const *data, u32 size) {
	// TODO: if machine is big endian convert bytes from big to little endian
	write(f, &size, sizeof(size));
	write(f, &tag, sizeof(tag));
	return size > 0 ? write(f, data, size) : 0;
}

i32 mrecv(int f, Tag *tag, void *data, u32 size) {
	// TODO: if machine is big endian convert bytes from little to big endian
	u32 message_size;
	read(f, &message_size, 4);
	if (message_size > size)
		panic("incoming message size %u exceeded maxiumum beffer size %u\n", message_size, size);
	read(f, tag, 1);
	return size > 0 ? read(f, data, message_size) : 0;
}

i32 mrecvt(int f, Tag tag, void *data, u32 size) {
	// TODO: wait for message with tag `tag`
	return -1;
}
