#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

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
    if (end.tv_nsec < start.tv_nsec) {
        temp.tv_sec  = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec  = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

inline bool time_less_than(timespec a, timespec b) {
    timespec res = time_diff(a, b);
    return res.tv_sec < 0 || res.tv_nsec < 0;
}

typedef struct {
    int in;
    int out;
    Pentago game;
    PentagoMove *stack;
} Player;

typedef enum {
    PLAYER_OK      = 0,
    PLAYER_ILLEGAL = 1,
    PLAYER_TIMEOUT = 2,
} HandlePlayerResult;

HandlePlayerResult handle_player(Player *p, Pentago *game, f32 timeout) {
    int in  = p->in;
    int out = p->out;

    // prepare player's local copy of the game
    i32 size = game->board_size * game->board_size;
    memcpy(p->game.board, game->board, size);
    p->game.current_player = game->current_player;
    p->game.winner = game->winner;
    p->game.board_size = game->board_size;
    arrsetlen(p->stack, 0);

    bool done = false;
    while (!done) {
        i8 tag;
        u8 buffer[0x1000] = {};
        i32 received = mrecv(out, &tag, buffer, 0x1000);
        // TODO(piotr): check whether the received size matches the tag
        // TODO(piotr): check timeout

        switch (tag) {
            case MSG_COMMIT_MOVE: {
                u8 i, j, rotation;
                mscanf(buffer, "%u %u %u", &i, &j, &rotation);
                printf("Player %c: (%u, %u) %u\n", game->current_player, i, j, rotation);
                PentagoError err = make_move(game, i, j, rotation);
                if (err) {
                    printf("MSG_COMMIT_MOVE error %d\n", err);
                    return PLAYER_ILLEGAL;
                }
                done = true;
            } break;

            case MSG_GET_MOVES: {
                PentagoMoves moves;
                moves = get_available_moves(&p->game);
                msendf(in, MSG_GET_MOVES, "%u[%] %u[%] %u[%]",
                        moves.i, moves.count,
                        moves.j, moves.count,
                        moves.rotation, moves.count);
                free_moves(&moves);
            } break;

            case MSG_MAKE_MOVE: {
                u8 i, j, rotation;
                mscanf(buffer, "%u %u %u", &i, &j, &rotation);
                int no_rotation = 0;
                make_move_(&p->game, i, j, rotation, &no_rotation);
                if (no_rotation) rotation = 8;
                PentagoMove move = {i, j, rotation};
                arrpush(p->stack, move);
            } break;

            case MSG_UNDO_MOVE: {
                if (arrlen(p->stack) == 0) {
                    puts("MSG_UNDO_MOVE error: no move to undo");
                    return PLAYER_ILLEGAL;
                }
                PentagoMove move = arrpop(p->stack);
                PentagoError err;
                err = undo_move(&p->game, move.i, move.j, move.rotation);
                if (err) {
                    printf("undoing move (%u, %u) %u\n", move.i, move.j, move.rotation);
                    printf("ERR %d\n", err);
                    return PLAYER_ILLEGAL;
                }
            } break;

            case MSG_GET_WINNER: {
                msendf(in, MSG_GET_WINNER, "%u", p->game.winner);
            } break;

            case MSG_GET_BOARD: {
                i32 res = msendf(in, MSG_GET_BOARD, "%u[%,%]", p->game.board,
                        p->game.board_size, p->game.board_size);
                //printf("sending board, sent %d bytes\n", res);
            } break;

            case MSG_GET_PLAYER: {
                msendf(in, MSG_GET_PLAYER, "%u", p->game.current_player);
            } break;
        }
    }

    return PLAYER_OK;
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

    // initialize a local copy of the game for each player
    Pentago p1_game, p2_game;
    pentago_create(&p1_game, board_size);
    pentago_create(&p2_game, board_size);

    Player p1 = {p1_in, p1_out, p1_game, NULL};
    Player p2 = {p2_in, p2_out, p2_game, NULL};

    // TODO(piotr): handle illegal behavior correctly
    HandlePlayerResult res;
    while (!game.winner) {
        res = handle_player(&p1, &game, timeout);
        board_print(&game);
        if (res) {
            puts("PLAYER 1 ILLEGAL");
            break;
        }
        if (game.winner) break;
        res = handle_player(&p2, &game, timeout);
        board_print(&game);
        if (res) {
            puts("PLAYER 2 ILLEGAL");
            break;
        }
    }
    // TODO(piotr): print result correctly
    printf("Winner: %c\n", game.winner);
}
