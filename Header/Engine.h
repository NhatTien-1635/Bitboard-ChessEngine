//
// Created by Hi on 4/18/2026.
//

#ifndef BITMAPMANIPULATOR_ENGINE_H
#define BITMAPMANIPULATOR_ENGINE_H

#include "ChessBoard.h"
#include "TranspositionTable.h"

#define PRINT_DEBUG

class Engine {
public:
    static int GetBestMove(ChessBoard &chess_board, int depth);

public:
    Engine() = delete;

    ~Engine() = default;

    static void PrintScoreMoves(const MoveList &move_list, const ChessBoard &chess_board);

public:
    static constexpr int max_ply = 128;

private:
    static int Negamax(int alpha, int beta, ChessBoard &chess_board, int depth, int ply);

    static int QuiescenceSearch(int alpha, int beta, ChessBoard &chess_board, int ply);

    static int ExtractPV(ChessBoard& chess_board, int depth);

private:
    static TranspositionTable hash_table;
    static int best_move;

    static int pv_table[max_ply];
    static int pv_length;

    static uint64_t node;
#ifdef PRINT_DEBUG
    static int best_score;
#endif
};


#endif //BITMAPMANIPULATOR_ENGINE_H
