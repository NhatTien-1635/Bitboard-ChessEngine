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

#define FEN_TEST_POSITION_1 "7k/p7/Pp5p/1Pp3pP/2Pp1pP1/3PpP2/4P3/K7 w - - 0 1"
#define FEN_TEST_POSITION_2 "8/8/8/8/8/8/8/8 w - - 0 0"

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
    // chess_board.PrintBoard();
    // std::cout << Evaluator::EvaluatePosition(chess_board) << '\n';
    //
    int best_move = Engine::GetBestMove(chess_board, 15);

    std::cout << "Best move: " << MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(best_move)) <<
            MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(best_move));

    // chess_board.PopulateMoveList(move_list);
    // move_list.PrintList();
    // move_list.ClearList();
    //
    // chess_board.PopulateCaptureMoveList(move_list);
    // move_list.PrintList();
    // move_list.ClearList();
    //
    // chess_board.PopulateQuietMove(move_list);
    // move_list.PrintList();
    // move_list.ClearList();


    // std::cout << chess_board.GetPositionHashKey();


    // PerformanceTest performance_test;
    // performance_test.RunDriver(chess_board, 5);
    // performance_test.PrintResult();
    // TranspositionTable::PrintTable();

    // MoveList move_list;

    // chess_board.MakeMove(Engine::GetBestMove(chess_board, 3));
    // chess_board.PrintBoard();
    // std::cout << MoveList::FormatMoveToString(0, Engine::GetBestMove(chess_board, 3));

    // chess_board.PopulateMoveList(move_list);


    return 0;
}
