#include <algorithm>

#include "../Header/Terminal.h"
#include "../Header/Bitmap.h"
#include "../Header/MoveGenerator.h"
#include "../Header/MagicNumberGenerator.h"
#include "../Header/ChessBoard.h"
#include "../Header/MoveList.h"
#include "../Header/PerformanceTest.h"

#include <random>

#define FEN_TEST_POSITION "k7/4P3/3n4/4P3/8/8/5p2/5KB1 w KQkq - 0 1"

int main() {
    MoveGenerator::InitGenerator();

    ChessBoard chess_board(FEN_STARTING_POSITION);
    PerformanceTest perft_test;

    perft_test.RunDriver(chess_board, 7);
    perft_test.PrintResult();

    return 0;
}