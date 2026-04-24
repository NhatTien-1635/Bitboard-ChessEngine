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
    TranspositionTable::InitHashTable();

    TranspositionTable table;
    MoveList move_list;

    ChessBoard chess_board(FEN_STARTING_POSITION);
    chess_board.PrintBoard();
    std::cout << MoveList::FormatMoveToString(0, Engine::GetBestMove(chess_board, 9));

    // std::cout << chess_board.GetPositionHashKey();


    // PerformanceTest performance_test;
    // performance_test.RunDriver(chess_board, 3);
    // performance_test.PrintResult();
    // TranspositionTable::PrintTable();

    // MoveList move_list;

    // chess_board.MakeMove(Engine::GetBestMove(chess_board, 3));
    // chess_board.PrintBoard();
    // std::cout << MoveList::FormatMoveToString(0, Engine::GetBestMove(chess_board, 3));

    // chess_board.PopulateMoveList(move_list);


    return 0;
}
