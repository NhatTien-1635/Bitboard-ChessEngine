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
    static int ScoreMove(int encoded_move, const ChessBoard& chess_board);

    //Return the best captured move in the list. then it remove it out of the list.
    static int SelectBestMove(MoveList& move_list, const ChessBoard& chess_board);

private:
    static int best_move;

#ifdef PRINT_NODE
    static int node;
#endif


    static constexpr int mvv_laa_table[6][6] = {
        #include "../Data/mvv_lva_data.dat"
    };
};


#endif //BITMAPMANIPULATOR_ENGINE_H
