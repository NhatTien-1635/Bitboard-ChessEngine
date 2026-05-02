//
// Created by Hi on 4/17/2026.
//

#ifndef BITMAPMANIPULATOR_EVALUATOR_H
#define BITMAPMANIPULATOR_EVALUATOR_H

#include "ChessBoard.h"
#include "Terminal.h"


class Evaluator {
public:
    static void InitializeEvaluationTable();

    static int EvaluatePosition(const ChessBoard &chess_board);

    //Return the best captured move in the list. then it remove it out of the list.
    static int SelectBestMove(MoveList &move_list);

    static void ScoreMoveList(MoveList &move_list, const ChessBoard &chess_board, int ply);

    static int ScoreMove(int encoded_move, const ChessBoard &chess_board, int ply);

    static void StoreKillerMove(int encoded_move, int ply) {
        killer_moves[1][ply] = killer_moves[0][ply];
        killer_moves[0][ply] = encoded_move;
    }

    static void UpdateHistoryMove(int encoded_move, int depth) {
        history_moves[MoveList::DecodeGetPiece(encoded_move)][MoveList::DecodeGetTargetSquare(encoded_move)] += depth;
    }

    static void ClearHistoryKillerMoveTable() {
        std::memset(history_moves, 0, sizeof(history_moves));
        std::memset(killer_moves, 0, sizeof(killer_moves));
    }

public:
    static constexpr int infinity_score = 50000;

public:
    Evaluator() = delete;

    ~Evaluator() = default;

private:
    static int GetScoreSEE(int encoded_move, const ChessBoard &chess_board);

    static int CalculateKingSafetyScore(const ChessBoard &chess_board, int king_square, Side side, bool mid_game);

private:
    //Configuration for evaluation
    static constexpr int midgame_double_pawn_penalty = -30;
    static constexpr int midgame_isolated_pawn_penalty = -40;
    static constexpr int midgame_passed_pawn_bonus[8] = {0, 10, 25, 50, 80, 120, 180, 300};

    static constexpr int endgame_double_pawn_penalty = -40;
    static constexpr int endgame_isolated_pawn_penalty = -50;
    static constexpr int endgame_passed_pawn_bonus[8] = {0, 30, 60, 120, 200, 300, 450, 600};

    static constexpr int midgame_semi_open_file_bonus = 25;
    static constexpr int endgame_semi_open_file_bonus = 40;

    static constexpr int midgame_open_file_bonus = 45;
    static constexpr int endgame_open_file_bonus = 60;

    static constexpr int midgame_shield_score[6] = {150, 110, 80, 30, -40, -50};
    static constexpr int endgame_shield_score[6] = {50, 30, 20, 10, 0, 0};

    static constexpr int midgame_vulnerability_penalty[6] = {60, 100, 160, 190, 230, 150};
    static constexpr int endgame_vulnerability_penalty[6] = {20, 40, 60, 100, 150, 100};

    static constexpr int king_in_check_penalty = -200;

    static constexpr double midgame_activity_multiplier = 0.015;
    static constexpr double endgame_activity_multiplier = 0.30;

    static constexpr int tempo = 10;

    /**
    *  All value were taken from Chess Programming WIKI
    *  URL: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
    */
    static int midgame_table[12][64];
    static int endgame_table[12][64];

    static constexpr int game_phase_table[12] = {0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0};

    static constexpr int midgame_value[6] = {82, 337, 365, 477, 1025, 0};
    static constexpr int endgame_value[6] = {94, 281, 297, 512, 936, 0};

    static constexpr int see_piece_value[6] = {100, 300, 300, 500, 900, 5000};

    static constexpr int mvv_laa_table[6][6] = {
#include "../Data/mvv_lva_data.dat"
    };

    //Killer move [id][ply]
    static int killer_moves[2][256];

    //History move [piece][square]
    static int history_moves[12][256];

    static Bitmap file_mask[64];
    static Bitmap rank_mask[64];

    static Bitmap isolated_pawn_mask[64];
    static Bitmap passed_pawn_mask[2][64];

    static Bitmap king_shield_mask[64];
    static Bitmap king_safety_zone_mask[64];

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
