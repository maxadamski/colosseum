#ifndef PENTAGO_INCLUDE
#define PENTAGO_INCLUDE

#include <stdint.h>

typedef enum {
    PERR_OK = 0,
    PERR_BAD_ARGUMENT = 1,
    PERR_ILLEGAL_MOVE = 2,
} PentagoError;

typedef enum {
    MSG_NONE      = 0,
    MSG_MAKE_MOVE = 1,
} PentagoMsg;

typedef struct {
    // board stored as string where:
    //     '.' is an empty field
    //     'B' is a black stone
    //     'W' is a white stone
    char *board;

    // boad_size of N denotes a board with NxN fields
    uint8_t board_size;

    // 'B' for blacks turn, 'W' for whites turn
    char current_player;

    //  0  if there's no winner yet
    // 'B' if black wins
    // 'W' if white wins
    // 'D' if it's a draw
    char winner;
} Pentago;

#define ROTATION_TOP_LEFT_CW   0b000
#define ROTATION_TOP_LEFT_CCW  0b001
#define ROTATION_TOP_RIGHT_CW  0b010
#define ROTATION_TOP_RIGHT_CCW 0b011
#define ROTATION_BOT_LEFT_CW   0b100
#define ROTATION_BOT_LEFT_CCW  0b101
#define ROTATION_BOT_RIGHT_CW  0b110
#define ROTATION_BOT_RIGHT_CCW 0b111

typedef struct {
    int32_t count;
    uint8_t *i;
    uint8_t *j;
    uint8_t *rotation;
} PentagoMoves;

// TODO(piotr): document

PentagoError pentago_create(Pentago *game, uint8_t size);
void pentago_destroy(Pentago *game);
char other_player(char player);
char * board_get(Pentago *game, uint8_t i, uint8_t j);
PentagoError make_move(Pentago *game, uint8_t i, uint8_t j, uint8_t rotation);
char get_winner(Pentago *game);
void board_print(Pentago *game);
PentagoMoves get_available_moves(Pentago *game);
void free_moves(PentagoMoves *moves);
#endif // PENTAGO_INCLUDE
