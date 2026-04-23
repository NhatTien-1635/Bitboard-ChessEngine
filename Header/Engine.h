//
// Created by Hi on 4/18/2026.
//

#ifndef BITMAPMANIPULATOR_ENGINE_H
#define BITMAPMANIPULATOR_ENGINE_H
#include "ChessBoard.h"

#define PRINT_NODE

class Engine {
public:
    static int GetBestMove(ChessBoard &chess_board, int depth);

public:
    Engine() = delete;

    ~Engine() = default;

    static void PrintScoreMoves(const MoveList& move_list, const ChessBoard& chess_board);

private:
    static int Negamax(int alpha, int beta, ChessBoard &chess_board, int depth, int ply);
    static int QuiescenceSearch(int alpha, int beta, ChessBoard &chess_board, int ply);

// private:
public:


private:
    static int best_move;

#ifdef PRINT_NODE
    static int node;
#endif
};


#endif //BITMAPMANIPULATOR_ENGINE_H
