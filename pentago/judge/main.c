#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "../pentago.c"

int main(int argc, char **argv) {
    if (argc != 6) {
        puts("usage: player/1/in player/1/out player/2/in player/2/out board_size");
        return 1;
    }
    char *p1_in  = argv[1];
    char *p1_out = argv[2];
    char *p2_in  = argv[3];
    char *p2_out = argv[4];
    int32_t board_size = atoi(argv[5]);

    // TODO(piotr): check the arguments
    // TODO(piotr): open player fifos

    Pentago game;
    pentago_create(&game, board_size);
}
