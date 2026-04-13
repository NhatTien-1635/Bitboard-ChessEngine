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

    static int EncodeMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece, int double_push_flag, int enpassant_flag, int castling_flag);

    static Square DecodeGetSourceSquare(uint64_t move);
    static Square DecodeGetTargetSquare(uint64_t move);
    static Piece DecodeGetPiece(uint64_t move);
    static Piece DecodeGetPromotedPiece(uint64_t move);
    static Piece DecodeGetCapturePiece(uint64_t move);
    static bool DecodeGetDoublePushFlag(uint64_t move);
    static bool DecodeGetEnpassantFlag(uint64_t move);
    static bool DecodeGetCastlingFlag(uint64_t move);

    void ClearList();
    void AddMove(int move);
    void AddMove(int source_square, int target_square, int piece, int promoted_piece, int captured_piece, int double_push_flag, int enpassant_flag, int castling_flag);
    int GetMove(int index) const;
    int GetMoveCount() const;

    MoveList() = default;
    ~MoveList() = default;

// private:
    static std::string FormatMoveToString(int index, int move);

private:
    uint64_t move_list[256];
    int count = 0;


};


#endif //BITMAPMANIPULATOR_MOVELIST_H