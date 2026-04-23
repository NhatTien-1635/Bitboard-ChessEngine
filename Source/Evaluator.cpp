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


int Evaluator::SelectBestMove(MoveList &move_list, const ChessBoard& chess_board) {
    int move_count = move_list.GetMoveCount();
    int best_index = 0;
    int max_score = -100000;

    for (int i = 0; i < move_count; ++i) {
        int current_score = ScoreMove(move_list.GetMove(i), chess_board);
        if (current_score > max_score) {
            max_score = current_score;
            best_index = i;
        }
    }

    move_list.Swap(best_index, move_count - 1);

    int move = move_list.GetMove(move_count - 1);
    move_list.PopBack();
    return move;
}

int Evaluator::ScoreMove(int encoded_move, const ChessBoard &chess_board) {
    if (MoveList::DecodeGetCapturePiece(encoded_move) == NoPiece) {
        return 0;
    }

    int see_score = GetScoreSEE(encoded_move, chess_board);
    if (see_score >= 0) {
        return 50000 + mvv_laa_table[MoveList::DecodeGetPiece(encoded_move) % 6][MoveList::DecodeGetCapturePiece(encoded_move) % 6];
    }
    else {
        return -50000 + mvv_laa_table[MoveList::DecodeGetPiece(encoded_move) % 6][MoveList::DecodeGetCapturePiece(encoded_move) % 6];
    }
}

int Evaluator::GetScoreSEE(int encoded_move, const ChessBoard &chess_board) {

    int from = MoveList::DecodeGetSourceSquare(encoded_move);
    int to = MoveList::DecodeGetTargetSquare(encoded_move);

    int gain[32]{};
    int depth = 0;

    Bitmap temp_occupancy = chess_board.GetOccupancyBitboard(Both);
    Side current_side = chess_board.CurrentSide();

    int attacker_piece = MoveList::DecodeGetPiece(encoded_move);
    int victim_piece = MoveList::DecodeGetCapturePiece(encoded_move);

    gain[0] = see_piece_value[victim_piece % 6];
    temp_occupancy.RemoveBit(from);

    victim_piece = attacker_piece;
    current_side = ChessBoard::opponent_side[current_side];

    while (depth < 31) {
        ++depth;

        int next_attacker_square = chess_board.GetWeakestAttackerSquare(to, current_side, temp_occupancy);

        if (next_attacker_square == NoSquare) {
            break;
        }

        attacker_piece = chess_board.At(next_attacker_square);
        gain[depth] = see_piece_value[victim_piece % 6] - gain[depth - 1];

        victim_piece = attacker_piece;
        temp_occupancy.RemoveBit(next_attacker_square);
        current_side = ChessBoard::opponent_side[current_side];

        if (std::max(-gain[depth - 1], gain[depth]) < 0) {
            break;
        }
    }

    while (--depth >= 0) {
        gain[depth - 1] = -std::max(-gain[depth - 1], gain[depth]);
    }
    return gain[0];
}
