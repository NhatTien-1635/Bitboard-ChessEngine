//
// Created by Hi on 4/17/2026.
//

#include "../Header/Evaluator.h"

int Evaluator::midgame_table[12][64]{};
int Evaluator::endgame_table[12][64]{};
int Evaluator::killer_moves[2][256]{};
int Evaluator::history_moves[12][256]{};

Bitmap Evaluator::file_mask[64]{};
Bitmap Evaluator::rank_mask[64]{};
Bitmap Evaluator::isolated_pawn_mask[64]{};
Bitmap Evaluator::passed_pawn_mask[2][64]{};


void Evaluator::InitializeEvaluationTable() {
    //Loop over every piece
    for (int piece = 0; piece < 12; ++piece) {
        //Loop over every piece
        for (int square = 0; square < 64; ++square) {
            if (piece < 6) {
                midgame_table[piece][square] = midgame_value[piece] + midgame_pesto_table[piece][square];
                endgame_table[piece][square] = endgame_value[piece] + endgame_pesto_table[piece][square];
            } else {
                midgame_table[piece][square] = midgame_value[piece % 6] + midgame_pesto_table[piece % 6][
                                                   ChessBoard::GetFlippedSquare(square)];
                endgame_table[piece][square] = endgame_value[piece % 6] + endgame_pesto_table[piece % 6][
                                                   ChessBoard::GetFlippedSquare(square)];
            }
        }
    }

    for (int square = 0; square < 64; ++square) {
        int file = square & 7;  //square & 0b0111
        int rank = square >> 3; //square / 8 (2^3)

        //Init file rank mask
        file_mask[square] = Bitmap::GetFileMask(file);
        rank_mask[square] = Bitmap::GetRankMask(rank);

        //Init isolated pawn mask
        if (file < 7) isolated_pawn_mask[square] |= Bitmap::GetFileMask(file + 1);
        if (file > 0) isolated_pawn_mask[square] |= Bitmap::GetFileMask(file - 1);

        //Init passed pawn mask
        passed_pawn_mask[White][square] = isolated_pawn_mask[square] | Bitmap::GetFileMask(file);
        passed_pawn_mask[Black][square] = isolated_pawn_mask[square] | Bitmap::GetFileMask(file);

        //Removing irrelevant rank
        for (int i_rank = 7; i_rank >= rank; --i_rank) {
            passed_pawn_mask[White][square] &= ~Bitmap::GetRankMask(i_rank);
        }

        for (int i_rank = 0; i_rank <= rank; ++i_rank) {
            passed_pawn_mask[Black][square] &= ~Bitmap::GetRankMask(i_rank);
        }
    }
}

int Evaluator::EvaluatePosition(const ChessBoard &chess_board) {
    int midgame[2] = {};
    int endgame[2] = {};
    int midgame_phase = 0;

    Bitmap bitboard;
    for (int piece = 0; piece < 12; ++piece) {
        bitboard = chess_board.GetPieceBitmap(piece);

        while (bitboard) {
            int square = bitboard.GetFirstLSBIndex();
            bitboard.RemoveBit(square);

            Side piece_side = Side(piece / 6);

            //Double pawn check
            if (piece == WhitePawn){
                Bitmap file_map = file_mask[square] & chess_board.GetPieceBitmap(WhitePawn);
                int pawn_count = file_map.GetBitCount();

                if (pawn_count > 1) {
                    midgame[piece_side] += midgame_double_pawn_penalty * (pawn_count - 1);  //We only add penalty to the extra pawns
                    endgame[piece_side] += endgame_double_pawn_penalty * (pawn_count - 1);
                }

                if (!(isolated_pawn_mask[square] & chess_board.GetPieceBitmap(WhitePawn))){
                    midgame[piece_side] += midgame_isolated_pawn_penalty;
                    endgame[piece_side] += endgame_isolated_pawn_penalty;
                }

                if (!(passed_pawn_mask[White][square] & chess_board.GetPieceBitmap(BlackPawn))) {
                    midgame[piece_side] += midgame_passed_pawn_bonus[7 - (square >> 3)];
                    endgame[piece_side] += endgame_passed_pawn_bonus[7 - (square >> 3)];
                }
            }

            if (piece == BlackPawn){
                Bitmap file_map = file_mask[square] & chess_board.GetPieceBitmap(BlackPawn);
                int pawn_count = file_map.GetBitCount();

                if (pawn_count > 1) {
                    midgame[piece_side] += midgame_double_pawn_penalty * (pawn_count - 1);
                    endgame[piece_side] += endgame_double_pawn_penalty * (pawn_count - 1);
                }

                if (!(isolated_pawn_mask[square] & chess_board.GetPieceBitmap(BlackPawn))){
                    midgame[piece_side] += midgame_isolated_pawn_penalty;
                    endgame[piece_side] += endgame_isolated_pawn_penalty;
                }

                if (!(passed_pawn_mask[Black][square] & chess_board.GetPieceBitmap(WhitePawn))) {
                    midgame[piece_side] += midgame_passed_pawn_bonus[square >> 3];
                    endgame[piece_side] += endgame_passed_pawn_bonus[square >> 3];
                }
            }

            midgame_phase += game_phase_table[piece];

            midgame[piece_side] += midgame_table[piece][square];
            endgame[piece_side] += endgame_table[piece][square];
        }
    }

    int midgame_score = midgame[chess_board.CurrentSide()] - midgame[ChessBoard::opponent_side[chess_board.
                            CurrentSide()]] + tempo;
    int endgame_score = endgame[chess_board.CurrentSide()] - endgame[ChessBoard::opponent_side[chess_board.
                            CurrentSide()]] + tempo;
    if (midgame_phase > 24) {
        midgame_phase = 24;
    }

    int endgame_phase = 24 - midgame_phase;
    return (midgame_score * midgame_phase + endgame_score * endgame_phase) / 24;
}


int Evaluator::SelectBestMove(MoveList &move_list) {
    int move_count = move_list.GetMoveCount();
    int best_index = 0;
    int max_score = -1000000;

    for (int i = 0; i < move_count; ++i) {
        if (max_score < move_list.GetScore(i)) {
            max_score = move_list.GetScore(i);
            best_index = i;
        }
    }

    move_list.Swap(move_count - 1, best_index);

    int move = move_list.GetMove(move_count - 1);
    move_list.PopBack();
    return move;
}

void Evaluator::ScoreMoveList(MoveList &move_list, const ChessBoard &chess_board, int ply) {
    int move_count = move_list.GetMoveCount();

    for (int i = 0; i < move_count; ++i) {
        move_list.AddScore(i, ScoreMove(move_list.GetMove(i), chess_board, ply));
    }
}

int Evaluator::ScoreMove(int encoded_move, const ChessBoard &chess_board, int ply) {
    if (MoveList::DecodeGetCapturePiece(encoded_move) != NoPiece) {
        int see_score = GetScoreSEE(encoded_move, chess_board);
        if (see_score < 0) {
            return see_score;
        }
        return 100000 + mvv_laa_table[MoveList::DecodeGetPiece(encoded_move) % 6][
                   MoveList::DecodeGetCapturePiece(encoded_move) % 6];
    }

    if (MoveList::DecodeGetPromotedPiece(encoded_move) != NoPiece) {
        return 100000;
    }

    if (encoded_move == killer_moves[0][ply]) {
        return 90000;
    }

    if (encoded_move == killer_moves[1][ply]) {
        return 80000;
    }

    return history_moves[MoveList::DecodeGetPiece(encoded_move)][MoveList::DecodeGetTargetSquare(encoded_move)];
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

    while (--depth > 0) {
        gain[depth - 1] = -std::max(-gain[depth - 1], gain[depth]);
    }
    return gain[0];
}
