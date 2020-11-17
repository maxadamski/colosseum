#ifndef PENTAGO_INCLUDE
#define PENTAGO_INCLUDE

#include <stdint.h>

typedef enum {
    PERR_OK = 0,
    PERR_BAD_ARGUMENT = 1,
    PERR_ILLEGAL_MOVE = 2,
} PentagoError;

typedef struct {
    // boad_size of N denotes a board with NxN fields
    int32_t board_size;

    // board stored as string where:
    //     '.' is an empty field
    //     'B' is a black stone
    //     'W' is a white stone
    char *board;

    // 'B' for blacks turn, 'W' for whites turn
    char current_player;

    //  0  if there's no winner yet
    // 'B' if black wins
    // 'W' if white wins
    // 'D' if it's a draw
    char winner;
} Pentago;

typedef struct {
    int32_t i;
    int32_t j;
    int32_t tile;
    char rotation;
} PentagoMove;

PentagoError pentago_create(Pentago *game, int32_t size);
void pentago_destroy(Pentago *game);
char other_player(char player);
char * board_get(Pentago *game, int32_t i, int32_t j);
PentagoError make_move(Pentago *game, int32_t i, int32_t j, int32_t tile, char rotation);
char get_winner(Pentago *game);

#endif // PENTAGO_INCLUDE
