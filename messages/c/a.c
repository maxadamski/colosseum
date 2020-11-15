#include "colosseum.h"
#include "stdio.h" 
#include "stdlib.h" 

int main(int argc, char **argv) {
	int f = open(argv[1], O_WRONLY);
	char const *msg = "hello, world!";
	tag_t tag = 42;
	msize_t size;
	while (1) {
		if ((size = msend_str(f, msg, tag)) < 0) perror("error: ");
		printf("--> %.*s [tag %d, size %d]\n", size, msg, tag, size);
	}
	close(f);
	return 0;
}
