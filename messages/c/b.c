#include "colosseum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	int f = open(argv[1], O_RDONLY);
	u32 const max_size = 1024;
	char msg[max_size];
	u32 msg_size;
	i32 number;

	while (1) {
		number = 0;
		msg[0] = '\0';

		timespec t0 = gettime();
		if (mrecvf(f, 42, "%I %u[%]", &number, msg, &msg_size) < 0) continue;
		timespec t1 = gettime();
		assert(number == 1024);
		assert(strcmp(msg, "Hello, World!") != 0);
		printf("recv (%ldns elapsed)\n", deltatime(t0, t1));
	}

	close(f);
	return 0;
}
