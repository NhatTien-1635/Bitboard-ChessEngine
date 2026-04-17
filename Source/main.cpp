#include "../Header/Terminal.h"
#include "../Header/Bitmap.h"
#include "../Header/MoveGenerator.h"
#include "../Header/MagicNumberGenerator.h"
#include "../Header/ChessBoard.h"
#include "../Header/MoveList.h"
#include "../Header/PerformanceTest.h"

#include <random>

#define FEN_TEST_POSITION "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "


int main() {
    MoveGenerator::InitGenerator();

    ChessBoard chess_board(FEN_STARTING_POSITION);
    chess_board.PrintBoard();
    PerformanceTest perft_test;

    perft_test.RunDriver(chess_board, 7);
    perft_test.PrintResult();

    // MoveList move_list;
    // chess_board.PopulateMoveList(move_list);
    // move_list.PrintList();
    // for (int i = 0; i < 40; ++i) {
    //     chess_board.MakeQuietMove(move_list.GetMove(0));
    //     move_list.ClearList();
    //     chess_board.PopulateMoveList(move_list);
    //     chess_board.PrintBoard();
    //     move_list.PrintList();
    // }

    return 0;
}
