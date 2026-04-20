#include "../Header/Terminal.h"
#include "../Header/Bitmap.h"
#include "../Header/MoveGenerator.h"
#include "../Header/MagicNumberGenerator.h"
#include "../Header/ChessBoard.h"
#include "../Header/MoveList.h"
#include "../Header/PerformanceTest.h"
#include "../Header/Evaluator.h"
#include "../Header/Engine.h"

#include <random>

#define FEN_TEST_POSITION "k7/8/K7/8/8/8/8/4Q3 w - - 0 0"


int main() {
    MoveGenerator::InitGenerator();
    Evaluator::InitializeEvaluationTable();

    ChessBoard chess_board(FEN_TRICKY_POSITION);
    chess_board.PrintBoard();

    // MoveList move_list;
    // chess_board.PopulateMoveList(move_list);
    // Engine::PrintScoreMoves(move_list);
    //
    // std::cout << "=====================================\n";
    //
    // move_list.ClearList();
    // chess_board.PopulateCaptureMoveList(move_list);
    // Engine::PrintScoreMoves(move_list);

    std::cout << MoveList::FormatMoveToString(0, Engine::GetBestMove(chess_board, 5));
    // Beep(750, 800);

    // std::cout << "Evaluated value: " << Evaluator::EvaluatePosition(chess_board);

    // PerformanceTest perft_test;

    // perft_test.RunDriver(chess_board, 5);
    // perft_test.PrintResult();

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
