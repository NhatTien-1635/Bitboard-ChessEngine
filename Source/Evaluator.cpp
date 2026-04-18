//
// Created by Hi on 4/17/2026.
//

#include "../Header/Evaluator.h"

int Evaluator::midgame_table[12][64];
int Evaluator::endgame_table[12][64];

void Evaluator::InitializeEvaluationTable() {
    //Loop over every piece
    for (int piece = 0; piece < 12; ++piece) {
        //Loop over every piece
        for (int square = 0; square < 64; ++square) {
            if (piece < 6) {
                midgame_table[piece][square] = midgame_value[piece] + midgame_pesto_table[piece][square];
                endgame_table[piece][square] = endgame_value[piece] + endgame_pesto_table[piece][square];
            } else {
                midgame_table[piece][square] = midgame_value[piece % 6] + midgame_pesto_table[piece % 6][ChessBoard::GetFlippedSquare(square)];
                endgame_table[piece][square] = endgame_value[piece % 6] + endgame_pesto_table[piece % 6][ChessBoard::GetFlippedSquare(square)];
            }
        }
    }
}

int Evaluator::EvaluatePosition(const ChessBoard &chess_board) {
    int midgame[2] = {};
    int endgame[2] = {};
    int midgame_phase = 0;

    for (int square = 0; square < 64; ++square) {
        Piece piece = chess_board.At(square);
        if (piece != NoPiece) {
            Side piece_side = Side(piece < 6);
            midgame_phase += game_phase_table[piece];

            midgame[piece_side] += midgame_table[piece][square];
            endgame[piece_side] += endgame_table[piece][square];
        }
    }

    int midgame_score = midgame[chess_board.CurrentSide()] - midgame[ChessBoard::opponent_side[chess_board.
                            CurrentSide()]];
    int endgame_score = endgame[chess_board.CurrentSide()] - endgame[ChessBoard::opponent_side[chess_board.
                            CurrentSide()]];
    if (midgame_phase > 24) {
        midgame_phase = 24;
    }

    int endgame_phase = 24 - midgame_phase;
    return (midgame_score * midgame_phase + endgame_score * endgame_phase) / 24;
}
