//
// Created by Hi on 4/18/2026.
//

#ifndef BITMAPMANIPULATOR_ENGINE_H
#define BITMAPMANIPULATOR_ENGINE_H

#include "ChessBoard.h"
#include "TranspositionTable.h"

#include <cmath>

//Debug macros
//==================================
// #define PRINT_DEBUG
#define SPEED_TEST
//==================================

#ifdef SPEED_TEST
#include <chrono>
#endif

class Engine {
public:
    static void InitTableLMR();

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

    static int ExtractPV(ChessBoard &chess_board, int depth);

private:
    static constexpr int full_depth_move_limit = 4;
    static constexpr int reduction_limit = 3;

    static constexpr int window_value = 50;
    static constexpr int window_widening_value = 150;

private:
    static TranspositionTable hash_table;
    static int best_move;

    static int pv_table[max_ply];
    static int pv_length;

    //lmr_table[depth][move_index]
    static int lmr_table[64][64];
    static constexpr double helper_constant = 2.2;

    static uint64_t node;

#ifdef PRINT_DEBUG
    static int best_score;
#endif

#ifdef SPEED_TEST
    static std::chrono::duration<double> run_time;
#endif
};


#endif //BITMAPMANIPULATOR_ENGINE_H
