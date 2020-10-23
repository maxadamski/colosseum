#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int in  = open("/fifo_in",  O_RDONLY);
    int out = open("/fifo_out", O_WRONLY);

    char buffer[256] = {};
    for (;;) {
        int count = read(in, buffer, 256);
        write(out, buffer, (size_t)count);
    }
}
