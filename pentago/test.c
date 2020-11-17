#include <stdio.h>
#include "pentago.c"

int tests = 0;
int passed = 0;
#define TEST(cond, name) {\
    tests++;\
    if (cond) passed++;\
    else puts("FAILED: " name);\
}

int main() {
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "......"
            "......"
            "BBBBB."
            "......"
            "......"
            "......";

        char winner = get_winner(&game);
        TEST(winner == 'B', "row winner");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "......"
            "..W..."
            "..W..."
            "..W..."
            "..W..."
            "..W...";

        char winner = get_winner(&game);
        TEST(winner == 'W', "column winner");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            ".W...."
            "..W..."
            "...W.."
            "....W."
            ".....W"
            "......";

        char winner = get_winner(&game);
        TEST(winner == 'W', "diagonal \\ winner 1");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "......"
            "W....."
            ".W...."
            "..W..."
            "...W.."
            "....W.";

        char winner = get_winner(&game);
        TEST(winner == 'W', "diagonal \\ winner 2");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "....B."
            "...B.."
            "..B..."
            ".B...."
            "B....."
            "......";

        char winner = get_winner(&game);
        TEST(winner == 'B', "diagonal / winner 1");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "......"
            ".....B"
            "....B."
            "...B.."
            "..B..."
            ".B....";

        char winner = get_winner(&game);
        TEST(winner == 'B', "diagonal / winner 2");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "WB...."
            "WB...."
            "WB...."
            "WB...."
            "WB...."
            "......";

        char winner = get_winner(&game);
        TEST(winner == 'D', "draw winner");
    }
    {
        Pentago game = {0};
        pentago_create(&game, 10);
        make_move(&game, 3, 2, 0, 'R');
        char res = *board_get(&game, 2, 1);
        pentago_destroy(&game);
        
        TEST(res == 'W', "rotate right");
    }
    {
        Pentago game = {0};
        pentago_create(&game, 10);
        make_move(&game, 1, 5, 1, 'L');
        char res = *board_get(&game, 4, 6);
        pentago_destroy(&game);
        
        TEST(res == 'W', "rotate left");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "......"
            "......"
            "......"
            "......"
            "......"
            "......";
        int32_t count = 0;
        PentagoMove *moves = get_available_moves(&game, &count);
        free(moves);
        TEST(count == 6*6*8, "empty board available moves");
    }
    {
        Pentago game = {0};
        game.board_size = 6;
        game.board = 
            "BWWBBW"
            "WWWBBB"
            "WWWB.B"
            "BBBWWW"
            "BBBWWW"
            "WBBWWB";
        int32_t count = 0;
        PentagoMove *moves = get_available_moves(&game, &count);
        int test = (count == 8 &&
                    moves[3].i == 2 &&
                    moves[3].j == 4 &&
                    moves[3].tile == 1 &&
                    moves[3].rotation == 'R');
        free(moves);
        TEST(test, "near full board available moves");
    }

    printf("%d / %d tests passed\n", passed, tests);
    if (passed == tests) {
        puts("All tests passed!");
    }

    return 0;
}
