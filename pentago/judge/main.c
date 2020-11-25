#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "pentago.c"
#include "colosseum.c"

typedef struct timespec timespec;

inline timespec get_time() {
    timespec temp;
    //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &temp);
    clock_gettime(CLOCK_MONOTONIC, &temp);
    return temp;
}

timespec time_diff(timespec start, timespec end) {
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

inline bool time_less_than(timespec a, timespec b) {
    timespec res = time_diff(a, b);
    return res.tv_sec < 0 || res.tv_nsec < 0;
}

void handle_player(int in, int out, Pentago *game, f32 timeout) {
    bool done = false;
    while (!done) {
        i8 tag;
        u8 buffer[0x1000] = {};
        i32 received = mrecv(out, &tag, buffer, 0x1000);
        // TODO(piotr): check whether the received size matches the tag

        switch (tag) {
            case MSG_COMMIT_MOVE: {
                u8 i, j, rotation;
                mscanf(buffer, "%u %u %u", &i, &j, &rotation);
                printf("Player %c: (%u, %u) %u\n", game->current_player, i, j, rotation);
                PentagoError err = make_move(game, i, j, rotation);
                if (err) {
                    game->winner = other_player(game->current_player);
                }
                done = true;
            } break;

            case MSG_GET_MOVES: {
                PentagoMoves moves = get_available_moves(game);
                if (moves.count == 0) return;
                msendf(in, MSG_GET_MOVES, "%u[%] %u[%] %u[%]",
                        moves.i, moves.count,
                        moves.j, moves.count,
                        moves.rotation, moves.count);
                free_moves(&moves);
            } break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 7) {
        puts("usage: player/1/in player/1/out player/2/in player/2/out board_size timeout");
        return 1;
    }
    int p1_in  = open(argv[1], O_WRONLY);
    int p1_out = open(argv[2], O_RDONLY);
    int p2_in  = open(argv[3], O_WRONLY);
    int p2_out = open(argv[4], O_RDONLY);
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
        handle_player(p1_in, p1_out, &game, timeout);
        board_print(&game);
        if (game.winner) break;
        handle_player(p2_in, p2_out, &game, timeout);
        board_print(&game);
    }
    printf("Winner: %c\n", game.winner);
}
