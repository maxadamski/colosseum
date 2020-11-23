#include "colosseum.h"

u8 const max_fields = 8;
u8 const max_dim = 8;
u32 const max_types = (4*max_dim+1)*max_fields;

typedef struct {
	u32 field_count, type_size, data_size;
} Metadata;

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

u32 parse_num(char const **fmt) {
	u32 res = 0;
	while ('0' <= **fmt && **fmt <= '9') {
		res = res * 10 + **fmt - '0';
		*fmt += 1;
	}
	return res;
}

void hexdump(u8 const *buffer, u32 size) {
	for (u32 i = 0; i < size; i++)
		printf("%02X ", buffer[i]);
}

void vmdecode(u8 const *data, char const *fmt, va_list args) {
	u8 const *types = data;
	u32 field = 0, type = 0;
	u32 sizes[max_fields];
	u8 *fields[max_fields];

	while (*fmt) {
		if (*fmt++ != '%') continue;
		if (field >= max_fields) panic("message must have at most %d fields, but found %d\n", max_fields, field);
		u8 msg_type = types[type++];
		u8 msg_base = msg_type >> 0 & 0x0F;
		u8 msg_dims = msg_type >> 4 & 0x07;
		u8 is_array = msg_type >> 7 & 0x01;
		u8 exp_type = format_type[(u8) *fmt++];
		sizes[field] = type_size[msg_base];
		fields[field] = va_arg(args, u8*);
		if (exp_type != msg_base)
			panic("message field %d expected base type %02X, but got %02X", field, exp_type, msg_base);
		if (*fmt == '[') {
			if (!is_array) panic("message field %d is not an array, found %02X\n", field, msg_base);
			u8 fmt_dims = 0;
			while (*(++fmt) != ']') {
				u32 dim = 0, max = 0;
				if (*fmt == '%') {
					fmt++;
					dim = *((u32*)(types + type));
					type += 4;
					if (fmt[0] == '<' && fmt[1] == '=') {
						fmt += 2;
						max = parse_num(&fmt);
					}
					*va_arg(args, u32*) = dim;
				} else {
					dim = max = parse_num(&fmt);
				}
				if (dim > max) panic("array dimimension %d must be less than %d, as specified in the format, but got %d\n", fmt_dims, max, dim);
				if (dim == 0 || max == 0) panic("array dimensions must be greater than 0, but got %d\n", dim);
				sizes[field] *= dim;
				if (*fmt == ']') break;
				if (*fmt != ',') panic("expected `,` but got `%c`\n", *fmt);
				fmt_dims++;
			}
			if (fmt_dims != msg_dims) panic("array dimensions don't match (%d != %d)\n", fmt_dims, msg_dims);
			if (fmt_dims == 0) panic("array must have at least one dimension\n");
			if (fmt_dims > max_dim) panic("array must have at most eight dimensions\n");
		}
		field++;
	}

	u32 offset = 0;
	for (int i = 0; i < field; i++) {
		for (int j = 0; j < sizes[i]; j++) {
			fields[i][j] = data[type + offset + j];
		}
		offset += sizes[i];
	}

#ifdef DEBUG
	printf("type "); hexdump(types, type); printf("\n");
	for (int i = 0; i < field; i++) {
		printf("arg%d ", i); hexdump(fields[i], sizes[i]); printf("\n");
	}
#endif
}

Metadata vmencode(u8 *fields[], u32 *sizes, u8 *types, char const *fmt, va_list args) {
	u8 type = 0, field = 0;
	u32 total = 0;
	while (*fmt) {
		if (*fmt++ != '%') continue;
		if (field >= max_fields) panic("message must have at most %d fields (found %d)\n", max_fields, field);
		u8 base = format_type[(u8) *fmt++];
		sizes[field] = type_size[base];
		u8 *arg = va_arg(args, u8*);
		if (*fmt == '[') {
			u32 dims[8];
			u8 n_dims = 0;
			while (*(++fmt) != ']') {
				u32 dim;
				if (*fmt == '%') {
					fmt++;
					dim = va_arg(args, u32);
				} else {
					dim = parse_num(&fmt);
				}
				if (dim == 0) panic("array dimensions must be greater than 0\n");
				dims[n_dims++] = dim;
				if (*fmt == ']') break;
				if (*fmt != ',') panic("expected `,` but got `%c`\n", *fmt);
			}
			if (n_dims == 0) panic("array must have at least one dimension\n");
			if (n_dims > max_dim) panic("array must have at most eight dimensions\n");
			types[type++] = 0x80 | (0x07 & n_dims) << 4 | (base & 0x0F);
			for (int i = 0; i < n_dims; i++) {
				u32 dim = dims[i];
				sizes[field] *= dim;
				*((u32*)(types + type)) = dim;
				type += 4;
			}
			fields[field] = arg;
		} else {
			types[type++] = base & 0x0F;
			fields[field] = (u8*) &arg;
		}
		total += sizes[field];
		field++;
	}

	return (Metadata){.field_count=field, .type_size=type, .data_size=total};
}

Metadata mencode(u8 *fields[], u32 *sizes, u8 *types, char const *fmt, ...) {
	va_list vargs;
	va_start(vargs, fmt);
	Metadata res = vmencode(fields, sizes, types, fmt, vargs);
	va_end(vargs);
	return res;
}

void mscanf(u8 *data, char const *fmt, ...) {
	va_list vargs;
	va_start(vargs, fmt);
	vmdecode(data, fmt, vargs);
	va_end(vargs);
}

i32 msendf(int f, Tag tag, char const *fmt, ...) {
	u8 *fields[max_fields];
	u32 sizes[max_fields];
	u8 types[max_types];
	va_list vargs;
	va_start(vargs, fmt);
	Metadata res = vmencode(fields, sizes, types, fmt, vargs);
	va_end(vargs);
	u32 const size = res.type_size + res.data_size;

	//u32 const total_size = 9 + res.type_size + res.data_size;
	//u8 buffer[total_size];

	u32 sent = 0;
	sent += write(f, &size, 4);
	sent += write(f, &tag, 1);
	sent += write(f, &res.type_size, 4);
	sent += write(f, types, res.type_size);
	for (int i = 0; i < res.field_count; i++)
		sent += write(f, fields[i], sizes[i]);

#ifdef DEBUG
	printf("type "); hexdump(types, res.type_size); printf("\n");
	for (int i = 0; i < res.field_count; i++) {
		printf("arg%d ", i); hexdump(fields[i], sizes[i]); printf("\n");
	}
#endif

	return sent;
}

i32 msend(int f, Tag tag, void const *data, u32 size) {
	// TODO: if machine is big endian convert bytes from big to little endian
	write(f, &size, 4);
	write(f, &tag, 1);
	return size > 0 ? write(f, data, size) : 0;
}

i32 mrecv(int f, Tag *tag, void *data, u32 max_size) {
	// TODO: if machine is big endian convert bytes from little to big endian
	u32 size;
	u32 recv = 0;
	u32 type_size = 0;
	recv += read(f, &size, 4);
	recv += read(f, tag, 1);
	recv += read(f, &type_size, 4);
	if (size > max_size)
		debug("incoming message size %u exceeded maximum buffer size %u\n", size, max_size);
	u32 data_size = size > 0 ? read(f, data, size) : 0;
	if (data_size < size)
		panic("expected payload of size %u, but only received %u bytes\n", size, data_size);
#ifdef DEBUG
	if (data_size) {
		printf("data "); hexdump(data, data_size); printf("\n");
	}
#endif
	return recv + data_size;
}

