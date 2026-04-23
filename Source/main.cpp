#include "../Header/Terminal.h"
#include "../Header/Bitmap.h"
#include "../Header/MoveGenerator.h"
#include "../Header/MagicNumberGenerator.h"
#include "../Header/ChessBoard.h"
#include "../Header/PerformanceTest.h"
#include "../Header/Evaluator.h"
#include "../Header/Engine.h"
#include "../Header/TranspositionTable.h"

#include <random>

#define FEN_TEST_POSITION "k7/8/K7/8/8/8/8/4Q3 w - - 0 0"


int main() {
    MoveGenerator::InitGenerator();
    Evaluator::InitializeEvaluationTable();
    TranspositionTable::InitTable();

    ChessBoard chess_board(FEN_TRICKY_POSITION);
    chess_board.PrintBoard();

    PerformanceTest performance_test;
    performance_test.RunDriver(chess_board, 4);
    performance_test.PrintResult();
    // TranspositionTable::PrintTable();

    // MoveList move_list;
    // std::cout << MoveList::FormatMoveToString(0, Engine::GetBestMove(chess_board, 5));
    // chess_board.PopulateMoveList(move_list);


    return 0;
}
