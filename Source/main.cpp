#include "../Header/Terminal.h"
#include "../Header/Bitmap.h"
#include "../Header/MoveGenerator.h"
#include "../Header/MagicNumberGenerator.h"
#include "../Header/ChessBoard.h"
#include "../Header/MoveList.h"

#include <random>

#define FEN_TEST_POSITION "k7/4P3/3n4/4P3/8/8/5p2/5KB1 w KQkq - 0 1"

int main() {
    MoveGenerator::InitGenerator();

    ChessBoard chess_board(FEN_TRICKY_POSITION);
    chess_board.PrintBoard();
    // chess_board.PrintAttack(Black);
    // chess_board.PrintAttack(White);


    // int move = MoveList::EncodeMove(e7, e8, WhitePawn, WhiteQueen, false, false, false, false);
    MoveList move_list;

    chess_board.PopulateMoveList(move_list);
    move_list.PrintList();


    return 0;
}
