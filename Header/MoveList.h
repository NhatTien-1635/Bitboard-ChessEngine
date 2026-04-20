//
// Created by Hi on 4/2/2026.
//

#ifndef BITMAPMANIPULATOR_MOVELIST_H
#define BITMAPMANIPULATOR_MOVELIST_H

#include "ChessBoard.h"
#include "MoveGenerator.h"

#include <stdint.h>

#include <iomanip>
#include <string.h> //memset
#include <sstream>
#include <string>

enum Piece : uint8_t;
class ChessBoard;

#define ENCODE_SOURCE_SQUARE_OFFSET     0
#define ENCODE_TARGET_SQUARE_OFFSET     6
#define ENCODE_PIECE_OFFSET             12
#define ENCODE_CAPTURED_PIECE_OFFSET    16
#define ENCODE_PROMOTED_PIECE_OFFSET    20
#define ENCODE_DOUBLE_PUSH_FLAG_OFFSET  24
#define ENCODE_ENPASSANT_FLAG_OFFSET    25
#define ENCODE_CASTLING_FLAG_OFFSET     26

#define DECODE_SOURCE_SQUARE_MASK     ((uint64_t)0x3F << ENCODE_SOURCE_SQUARE_OFFSET)
#define DECODE_TARGET_SQUARE_MASK     ((uint64_t)0x3F << ENCODE_TARGET_SQUARE_OFFSET)
#define DECODE_PIECE_MASK             ((uint64_t)0xF  << ENCODE_PIECE_OFFSET)
#define DECODE_CAPTURED_PIECE_MASK    ((uint64_t)0xF  << ENCODE_CAPTURED_PIECE_OFFSET)
#define DECODE_PROMOTED_PIECE_MASK    ((uint64_t)0xF  << ENCODE_PROMOTED_PIECE_OFFSET)
#define DECODE_DOUBLE_PUSH_FLAG_MASK  ((uint64_t)0x1  << ENCODE_DOUBLE_PUSH_FLAG_OFFSET)
#define DECODE_ENPASSANT_FLAG_MASK    ((uint64_t)0x1  << ENCODE_ENPASSANT_FLAG_OFFSET)
#define DECODE_CASTLING_FLAG_MASK     ((uint64_t)0x1  << ENCODE_CASTLING_FLAG_OFFSET)


class MoveList {
public:
    void PrintList();

    inline static int EncodeMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece, int double_push_flag, int enpassant_flag, int castling_flag);
    inline static Square DecodeGetSourceSquare(int move);
    inline static Square DecodeGetTargetSquare(int move);
    inline static Piece DecodeGetPiece(int move);
    inline static Piece DecodeGetPromotedPiece(int move);
    inline static Piece DecodeGetCapturePiece(int move);
    inline static bool DecodeGetDoublePushFlag(int move);
    inline static bool DecodeGetEnpassantFlag(int move);
    inline static bool DecodeGetCastlingFlag(int move);
    inline void ClearList();
    inline void AddMove(int move);
    inline void AddMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece, int double_push_flag, int enpassant_flag, int castling_flag);
    inline int GetMove(int index) const;
    inline int GetMoveCount() const;
    inline void Swap(int src, int tar);
    inline void PopBack(){ --count; };

    MoveList() = default;
    ~MoveList() = default;

// private:
    static std::string FormatMoveToString(int index, int move);

private:
    int move_list[256];
    int count = 0;


};

//
// Created by Hi on 4/2/2026.
//

int MoveList::EncodeMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece,
                         int double_push_flag, int enpassant_flag, int castling_flag) {
    uint64_t encoded_move = 0ULL;

    encoded_move |= source_square;
    encoded_move |= (target_square << ENCODE_TARGET_SQUARE_OFFSET);
    encoded_move |= (piece << ENCODE_PIECE_OFFSET);
    encoded_move |= (promoted_piece << ENCODE_PROMOTED_PIECE_OFFSET);
    encoded_move |= (captured_piece << ENCODE_CAPTURED_PIECE_OFFSET);
    encoded_move |= (double_push_flag << ENCODE_DOUBLE_PUSH_FLAG_OFFSET);
    encoded_move |= (enpassant_flag << ENCODE_ENPASSANT_FLAG_OFFSET);
    encoded_move |= (castling_flag << ENCODE_CASTLING_FLAG_OFFSET);

    return encoded_move;
}

Square MoveList::DecodeGetSourceSquare(int move) {
    return (Square) (move & DECODE_SOURCE_SQUARE_MASK);
}

Square MoveList::DecodeGetTargetSquare(int move) {
    return (Square) ((move & DECODE_TARGET_SQUARE_MASK) >> ENCODE_TARGET_SQUARE_OFFSET);
}

Piece MoveList::DecodeGetPiece(int move) {
    return (Piece) ((move & DECODE_PIECE_MASK) >> ENCODE_PIECE_OFFSET);
}

Piece MoveList::DecodeGetPromotedPiece(int move) {
    return (Piece) ((move & DECODE_PROMOTED_PIECE_MASK) >> ENCODE_PROMOTED_PIECE_OFFSET);
}

Piece MoveList::DecodeGetCapturePiece(int move) {
    return (Piece) ((move & DECODE_CAPTURED_PIECE_MASK) >> ENCODE_CAPTURED_PIECE_OFFSET);
}

bool MoveList::DecodeGetDoublePushFlag(int move) {
    return (move & DECODE_DOUBLE_PUSH_FLAG_MASK);
}

bool MoveList::DecodeGetEnpassantFlag(int move) {
    return (move & DECODE_ENPASSANT_FLAG_MASK);
}

bool MoveList::DecodeGetCastlingFlag(int move) {
    return (move & DECODE_CASTLING_FLAG_MASK);
}

void MoveList::ClearList() {
    memset(move_list, 0, sizeof(int) * 256);
    count = 0;
}

void MoveList::AddMove(int move) {
    move_list[count++] = move;
}

void MoveList::AddMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece,
                       int double_push_flag, int enpassant_flag, int castling_flag) {
    AddMove(EncodeMove(source_square, target_square, piece, promoted_piece, captured_piece, double_push_flag,
                       enpassant_flag, castling_flag));
}

int MoveList::GetMove(int index) const {
    return move_list[index];
}

int MoveList::GetMoveCount() const {
    return count;
}

void MoveList::Swap(int src, int tar) {
    int temp = move_list[src];
    move_list[src] = move_list[tar];
    move_list[tar] = temp;
}



#endif //BITMAPMANIPULATOR_MOVELIST_H