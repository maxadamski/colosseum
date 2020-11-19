#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "pentago.c"
#include "colosseum.c"

void handle_player(int in, int out, f32 timeout) {
    while (1) {
        Tag tag;
        u8 buffer[0x1000] = {};
        i32 received = mrecv(out, &tag, buffer, 0x1000);
        //assert(tag == MSG_MAKE_MOVE);
        printf("tag: %d\n", tag);
        for (i32 i = 0; i < received; i++) {
            printf("%02x ", buffer[i]);
        }
        puts("");
        break;
    }
}

int main(int argc, char **argv) {
    if (argc != 7) {
        puts("usage: player/1/in player/1/out player/2/in player/2/out board_size timeout");
        return 1;
    }
    int p1_in;//  = open(argv[1], O_WRONLY);
    int p1_out = open(argv[2], O_RDONLY);
    int p2_in;//  = open(argv[3], O_WRONLY);
    int p2_out;// = open(argv[4], O_RDONLY);
    u8 board_size = atoi(argv[5]);
    f32 timeout = atof(argv[6]);
    printf("board_size: %d\ntimeout: %f\n", (i32)board_size, timeout);

    // TODO(piotr): check the arguments

    Pentago game;
    PentagoError err = pentago_create(&game, board_size);
    if (err) {
        return 1;
    }

    while (!game.winner) {
        handle_player(p1_in, p1_out, timeout);
        return 0;
        if (game.winner) break;
        handle_player(p2_in, p2_out, timeout);
    }
}
