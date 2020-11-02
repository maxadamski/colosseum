#include "colosseum.h"
#include "stdio.h" 
#include "stdlib.h"

int main(int argc, char **argv) {
	int f = open(argv[1], O_RDONLY);
	char msg[1024];
	tag_t tag;
	msize_t size;
	while (1) {
		if ((size = mrecv(f, msg, &tag)) < 0) perror("error: ");
		if (!size) continue;
		printf("<-- %.*s [tag %d, size %d]\n", size, msg, tag, size);
		usleep(100);
	}
	close(f);
	return 0;
}
