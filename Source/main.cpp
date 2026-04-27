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

#define FEN_TEST_POSITION "8/1p6/8/8/7P/3p1p1p/p1prpp1P/k1Kbbrn1 w - - 0 70"


int main() {
    MoveGenerator::InitGenerator();
    Evaluator::InitializeEvaluationTable();
    TranspositionTable::InitHashTable();
    Engine::InitTableLMR();

    // TranspositionTable table;
    MoveList move_list;

    /**
     *  Early game:
     *  - Before: 1651968
     *  - After: 1883577
     *
     *  Mid game:
     *  - Before: 3433449
     *  - After: 3629711
     *
     *  End game:
     *  - Before:
     *  - After:
     */
    ChessBoard chess_board(FEN_EARLY_GAME);
    int best_move = Engine::GetBestMove(chess_board, 15);

    std::cout << "Best move: " << MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(best_move)) <<
            MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(best_move));


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
