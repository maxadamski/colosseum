#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "pentago.c"
#include "colosseum.c"

int main(int argc, char **argv) {
    int msgout  = open(argv[1], O_WRONLY);

    i32 res = msendf(msgout, MSG_MAKE_MOVE, "%b %i %i", (u8)1, (i8)2, (i8)3);
}
