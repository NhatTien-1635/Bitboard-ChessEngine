//
// Created by Hi on 4/17/2026.
//

#ifndef BITMAPMANIPULATOR_EVALUATOR_H
#define BITMAPMANIPULATOR_EVALUATOR_H

#include "ChessBoard.h"

class Evaluator {
public:
    static void InitializeEvaluationTable();

    static int EvaluatePosition(const ChessBoard &chess_board);

    //Return the best captured move in the list. then it remove it out of the list.
    static int SelectBestMove(MoveList &move_list, const ChessBoard &chess_board);

    static int ScoreMove(int encoded_move, const ChessBoard &chess_board);

public:
    Evaluator() = delete;

    ~Evaluator() = default;

private:
    static int GetScoreSEE(int encoded_move, const ChessBoard &chess_board);

    /**
     *  All value were taken from Chess Programming WIKI
     *  URL: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
     */
private:
    static int midgame_table[12][64];
    static int endgame_table[12][64];

    static constexpr int game_phase_table[12] = {0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0};

    static constexpr int midgame_value[6] = {82, 337, 365, 477, 1025, 0};
    static constexpr int endgame_value[6] = {94, 281, 297, 512, 936, 0};

    static constexpr int see_piece_value[6] = {100, 300, 300, 500, 900, 40000};

    static constexpr int mvv_laa_table[6][6] = {
#include "../Data/mvv_lva_data.dat"
    };

    static constexpr int midgame_pawn_value[64] = {
#include "../Data/Evaluation_Value/MidgamePawnValue.dat"
    };

    static constexpr int endgame_pawn_value[64] = {
#include "../Data/Evaluation_Value/EndgamePawnValue.dat"
    };

    static constexpr int midgame_knight_value[64] = {
#include "../Data/Evaluation_Value/MidgameKnightValue.dat"
    };

    static constexpr int endgame_knight_value[64] = {
#include "../Data/Evaluation_Value/EndgameKnightValue.dat"
    };

    static constexpr int midgame_bishop_value[64] = {
#include "../Data/Evaluation_Value/MidgameBishopValue.dat"
    };

    static constexpr int endgame_bishop_value[64] = {
#include "../Data/Evaluation_Value/EndgameBishopValue.dat"
    };

    static constexpr int midgame_rook_value[64] = {
#include "../Data/Evaluation_Value/MidgameRookValue.dat"
    };

    static constexpr int endgame_rook_value[64] = {
#include "../Data/Evaluation_Value/EndgameRookValue.dat"
    };

    static constexpr int midgame_queen_value[64] = {
#include "../Data/Evaluation_Value/MidgameQueenValue.dat"
    };

    static constexpr int endgame_queen_value[64] = {
#include "../Data/Evaluation_Value/EndgameQueenValue.dat"
    };

    static constexpr int midgame_king_value[64] = {
#include "../Data/Evaluation_Value/MidgameKingValue.dat"
    };

    static constexpr int endgame_king_value[64] = {
#include "../Data/Evaluation_Value/EndgameKingValue.dat"
    };

    static constexpr const int *midgame_pesto_table[6] = {
        midgame_pawn_value,
        midgame_knight_value,
        midgame_bishop_value,
        midgame_rook_value,
        midgame_queen_value,
        midgame_king_value
    };

    static constexpr const int *endgame_pesto_table[6] = {
        endgame_pawn_value,
        endgame_knight_value,
        endgame_bishop_value,
        endgame_rook_value,
        endgame_queen_value,
        endgame_king_value
    };
};


#endif //BITMAPMANIPULATOR_EVALUATOR_H
