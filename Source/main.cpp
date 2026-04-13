#include <algorithm>

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

    for (int index = 0; index < move_list.GetMoveCount(); ++index) {
        int move = move_list.GetMove(index);

        //Print move
        std::ios_base::fmtflags original_flags = std::cout.flags();
        std::cout << std::left
                  << std::setw(5)  << "ID"
                  << std::setw(10) << "Move"
                  << std::setw(10) << "Piece"
                  << std::setw(12) << "Promotion"
                  << std::setw(10) << "Capture"
                  << std::setw(12) << "Double"
                  << std::setw(12) << "Enpassant"
                  << std::setw(10) << "Castle\n";
        std::cout << MoveList::FormatMoveToString(index, move);
        std::cout.flags(original_flags);

        //Print board before make move
        std::cout << "\n________________________________________________\n";
        std::cout << "Board before make move:\n";
        chess_board.PrintBoard();

        //Print board after make move
        std::cout << "\n________________________________________________\n";
        std::cout << "Board after make move\n";
        chess_board.MakeQuietMove(move);
        chess_board.PrintBoard();

        //Print board after un-make move
        std::cout << "\n________________________________________________\n";
        std::cout << "Board after un-make move\n";
        chess_board.UnmakeMove(move);
        chess_board.PrintBoard();


    }

    return 0;
}
