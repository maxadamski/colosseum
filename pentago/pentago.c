#include "pentago.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


PentagoError pentago_create(Pentago *game, int32_t size) {
    if (size < 2 || size & 1 || !(size & 2)) {
        return PERR_BAD_ARGUMENT;
    }
    game->board_size = size;
    game->board = (char *)malloc(size*size + 1);
    memset(game->board, '.', size*size);
    game->board[size*size] = 0;

    game->current_player = 'W';
    game->winner = 0;

    return PERR_OK;
}

void pentago_destroy(Pentago *game) {
    free(game->board);
    game->board = NULL;
}

char other_player(char player) {
    assert(player == 'B' || player == 'W');
    return (player == 'B') ? 'W' : 'B';
}

char * board_get(Pentago *game, int32_t i, int32_t j) {
    return &game->board[i * game->board_size + j];
}

// rotate one layer of the tile
void rotate_square_left(Pentago *game, int32_t i, int32_t j, int32_t size) {
    // for each cycle in the square, rotate the cycle
    for (int32_t d = 0; d < size-1; d++) {
        char tmp = *board_get(game, i, j+d);
        *board_get(game, i,          j+d)        = *board_get(game, i+d,        j+size-1);
        *board_get(game, i+d,        j+size-1)   = *board_get(game, i+size-1,   j+size-1-d);
        *board_get(game, i+size-1,   j+size-1-d) = *board_get(game, i+size-1-d, j);
        *board_get(game, i+size-1-d, j)          = tmp;
    }
}

// rotate one layer of the tile
void rotate_square_right(Pentago *game, int32_t i, int32_t j, int32_t size) {
    // for each cycle in the square, rotate the cycle
    for (int32_t d = 0; d < size-1; d++) {
        char tmp = *board_get(game, i, j+d);
        *board_get(game, i,          j+d)        = *board_get(game, i+size-1-d, j);
        *board_get(game, i+size-1-d, j)          = *board_get(game, i+size-1,   j+size-1-d);
        *board_get(game, i+size-1,   j+size-1-d) = *board_get(game, i+d,        j+size-1);
        *board_get(game, i+d,        j+size-1)   = tmp;
    }
}

void board_rotate(Pentago *game, int32_t tile, char rotation) {
    int32_t tile_size = game->board_size / 2;
    int32_t i = 0; // top-left tile i
    int32_t j = 0; // top-left tile j
    if (tile > 1) i += tile_size;
    if (tile & 1) j += tile_size;

    while (tile_size > 1) {
        // rotate one layer of the tile

        if (rotation == 'L') {
            rotate_square_left(game, i, j, tile_size);
        } else {
            rotate_square_right(game, i, j, tile_size);
        }

        // go to next layer inward
        tile_size -= 2;
        i++; j++;
    }
}

PentagoError make_move(Pentago *game, int32_t i, int32_t j, int32_t tile, char rotation) {
    // TODO(piotr): more error checking
    char *field = board_get(game, i, j);
    if (*field != '.' || game->winner) {
        return PERR_ILLEGAL_MOVE;
    }
    char current = game->current_player;
    *field = current;
    if (get_winner(game) == game->current_player) {
        game->winner = game->current_player;
        return PERR_OK;
    }
    board_rotate(game, tile, rotation);
    game->current_player = other_player(current);
    game->winner = get_winner(game);
}

// helper function for get_winner
void process_consecutive(int32_t *count, char *color, int32_t *white_wins,
        int32_t *black_wins, int32_t winning_length, char c) {
    if (c == *color) {
        (*count)++;
        if (*count == winning_length) {
            if (*color == 'W') (*white_wins)++;
            if (*color == 'B') (*black_wins)++;
        }
    } else {
        *color = c;
        *count = 1;
    }
}

char get_winner(Pentago *game) {
    if (game->winner) return game->winner;
    int32_t size = game->board_size;
    int32_t winning_length = size/2 + 2;
    int32_t white_wins = 0;
    int32_t black_wins = 0;
    // check rows
    for (int32_t i = 0; i < size; i++) {
        int32_t count = 0;
        char color = '.';
        for (int32_t j = 0; j < size; j++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, i, j));
        }
    }
    // check columns
    for (int32_t j = 0; j < size; j++) {
        int32_t count = 0;
        char color = '.';
        for (int32_t i = 0; i < size; i++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, i, j));
        }
    }
    // check / diagonals, top-left half
    for (int32_t i = winning_length-1; i < size; i++) {
        int32_t count = 0;
        char color = '.';
        for (int32_t j = 0; i-j >= 0; j++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, i-j, j));
        }
    }
    // check / diagonals, bottom-right half
    for (int32_t j = 0; j <= size - winning_length; j++) {
        int32_t count = 0;
        char color = '.';
        // here i is the offset from the bottom edge of the board
        for (int32_t i = 0; i+j < size; i++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, size-1-i, i+j));
        }
    }
    // check \ diagonals, bottom-left half
    for (int32_t i = 0; i <= size - winning_length; i++) {
        int32_t count = 0;
        char color = '.';
        for (int32_t j = 0; i+j < size; j++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, i+j, j));
        }
    }
    // check \ diagonals, top-right half
    for (int32_t j = 0; j <= size - winning_length; j++) {
        int32_t count = 0;
        char color = '.';
        for (int32_t i = 0; i+j < size; i++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, i, i+j));
        }
    }

    char result = 0;
    if      (white_wins && black_wins)  result = 'D';
    else if (white_wins && !black_wins) result = 'W';
    else if (!white_wins && black_wins) result = 'B';
    return result;
}

void board_print(Pentago *game) {
    for (int32_t i = 0; i < game->board_size; i++) {
        for (int32_t j = 0; j < game->board_size; j++) {
            putchar(*board_get(game, i, j));
        }
        putchar('\n');
    }
}

PentagoMove * get_available_moves(Pentago *game, int32_t *count) {
    if (game->winner) {
        // the game is finished, can't make any moves
        *count = 0;
        return NULL;
    }

    // first pass to count empty fields
    int32_t moves_available = 0;
    for (int32_t i = 0; i < game->board_size * game->board_size; i++) {
        // for each empty field there are 8 possible rotations
        if (game->board[i] == '.') moves_available += 8;
    }

    PentagoMove *result = malloc(sizeof(PentagoMove) * moves_available);

    int32_t moves_filled = 0;
    for (int32_t i = 0; i < game->board_size; i++) {
        for (int32_t j = 0; j < game->board_size; j++) {
            if (*board_get(game, i, j) == '.') {
                result[moves_filled++] = (PentagoMove){i, j, 0, 'L'};
                result[moves_filled++] = (PentagoMove){i, j, 0, 'R'};
                result[moves_filled++] = (PentagoMove){i, j, 1, 'L'};
                result[moves_filled++] = (PentagoMove){i, j, 1, 'R'};
                result[moves_filled++] = (PentagoMove){i, j, 2, 'L'};
                result[moves_filled++] = (PentagoMove){i, j, 2, 'R'};
                result[moves_filled++] = (PentagoMove){i, j, 3, 'L'};
                result[moves_filled++] = (PentagoMove){i, j, 3, 'R'};
            }
        }
    }

    assert(moves_available == moves_filled);
    *count = moves_available;
    return result;
}
