#include "colosseum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	int f = open(argv[1], O_RDONLY);

	// message loop
	u8 buf[4096];
	i8 tag = 0;
	while (1) {
		if (mrecv(f, &tag, buf, 4096) <= 0) continue;
		printf("recv tag %d\n", tag);
		if (tag == 1) {
			u8 x, y, r;
			mscanf(buf, "%u %u %b", &x, &y, &r);
			printf("{x=%d, y=%d, r=%d}\n", x, y, r);
		} else if (tag == 42) {
			i32 code;
			char str[1024];
			u32 str_len;
			mscanf(buf, "%I %u[%<=10]", &code, str, &str_len);
			printf("{code=%d, len=%d, str='%s'}\n", code, str_len, str);
		}
	}

	close(f);
	return 0;
}
