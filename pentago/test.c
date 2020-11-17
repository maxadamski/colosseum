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
        make_move(&game, 3, 2, ROTATION_TOP_LEFT_CW);
        char res = *board_get(&game, 2, 1);
        pentago_destroy(&game);
        
        TEST(res == 'W', "rotate right");
    }
    {
        Pentago game = {0};
        pentago_create(&game, 10);
        make_move(&game, 1, 5, ROTATION_TOP_RIGHT_CCW);
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
        PentagoMoves moves = get_available_moves(&game);
        int32_t count = moves.count;
        free_moves(&moves);
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
        PentagoMoves moves = get_available_moves(&game);
        int test = moves.count == 8 &&
                   moves.i[3] == 2 &&
                   moves.j[3] == 4 &&
                   moves.rotation[3] == 3;
        free_moves(&moves);
        TEST(test, "near full board available moves");
    }

    printf("%d / %d tests passed\n", passed, tests);
    if (passed == tests) {
        puts("All tests passed!");
    }

    return 0;
}
