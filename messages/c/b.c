#include "colosseum.h"
#include "stdio.h" 
#include "stdlib.h"

int main(int argc, char **argv) {
	int f = open(argv[1], O_RDONLY);
	msize_t const buf_size = 1024;
	char msg[buf_size];
	tag_t tag;
	msize_t size;
	while (1) {
		if ((size = mrecv(f, msg, buf_size, &tag)) < 0) perror("error: ");
		if (!size) continue;
		printf("<-- %.*s [tag %d, size %d]\n", size, msg, tag, size);
	}
	close(f);
	return 0;
}
