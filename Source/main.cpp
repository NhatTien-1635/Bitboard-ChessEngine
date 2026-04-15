#include <algorithm>

#include "../Header/Terminal.h"
#include "../Header/Bitmap.h"
#include "../Header/MoveGenerator.h"
#include "../Header/MagicNumberGenerator.h"
#include "../Header/ChessBoard.h"
#include "../Header/MoveList.h"
#include "../Header/PerformanceTest.h"

#include <random>

#define FEN_TEST_POSITION "8/7K/8/7k/7p/8/6P1/8 w - - 0 1"

int main() {
    MoveGenerator::InitGenerator();

    ChessBoard chess_board(FEN_STARTING_POSITION);
    chess_board.PrintBoard();
    PerformanceTest perft_test;

    perft_test.RunDriver(chess_board, 6);
    perft_test.PrintResult();

    // MoveList move_list;
    // chess_board.PopulateMoveList(move_list);
    // move_list.PrintList();
    //
    // chess_board.MakeQuietMove(move_list.GetMove(30));
    // move_list.ClearList();
    // chess_board.PopulateMoveList(move_list);
    // chess_board.PrintBoard();
    // move_list.PrintList();

    return 0;
}