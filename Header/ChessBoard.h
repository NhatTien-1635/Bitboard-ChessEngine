//
// Created by Hi on 3/23/2026.
//

#ifndef BITMAPMANIPULATOR_CHESSBOARD_H
#define BITMAPMANIPULATOR_CHESSBOARD_H

#include <iostream>
#include <iomanip>
#include <string.h>

#include "MoveGenerator.h"
#include "TranspositionTable.h"

class TranspositionTable;

/**
 *  FEN Position
 *  Reference: https://www.chessprogramming.org/Forsyth-Edwards_Notation
 */

// #define HASH_KEY_DEBUG

//Debug test
#define FEN_EMPTY_BOARD "8/8/8/8/8/8/8/8 w - - "
#define FEN_STARTING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_TRICKY_POSITION "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN_KILLER_POSITION "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define FEN_CMK_POSITION "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

// 1. EARLY GAME (Quiet, developing phase)
// A standard Ruy Lopez opening after 5 moves.
#define FEN_EARLY_GAME "r1bqkbnr/1ppp1ppp/p1n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 4"

// 2. MID GAME (Highly tactical, chaotic)
#define FEN_MID_GAME FEN_TRICKY_POSITION

// 3. LATE GAME (Endgame, deep calculation but low branching factor)
#define FEN_END_GAME "8/8/8/7k/8/8/4K1Q1/8 w - - 0 1"

class MoveList;

enum Piece : uint8_t {
    WhitePawn = 0,
    WhiteKnight,
    WhiteBishop,
    WhiteRook,
    WhiteQueen,
    WhiteKing,
    BlackPawn,
    BlackKnight,
    BlackBishop,
    BlackRook,
    BlackQueen,
    BlackKing,
    NoPiece
};

enum CastlingRight : uint8_t {
    NoCastle = 0,
    WhiteToKing = 1,
    WhiteToQueen = 2,
    BlackToKing = 4,
    BlackToQueen = 8
};

struct BoardState {
    uint8_t side_to_move = White;
    uint8_t en_passant = NoSquare;
    uint8_t castling_right = 0b1111;

    uint32_t turn_count = 0;
    uint32_t half_clock = 0;

    uint64_t hash_key = 0;
};

class ChessBoard {
public:
    void PrintBoard() const;

    void PrintAttack(Side side) const;

    void ParsePositionFromFEN(std::string_view position);

    bool MakeMove(int encoded_move);

    bool MakeCaptureMove(int encoded_move);

    void MakeNullMove();

    void UnmakeMove(int encoded_move);

    void UnmakeNullMove();

    bool HasMajorPieceLeft(Side side) const;

    //Return if the square is attacked by the opposite color
    bool IsSquaredAttacked(uint8_t square, uint8_t attacker) const;

    int GetWeakestAttackerSquare(uint8_t square, uint8_t attacker, const Bitmap &occupancy) const;

    void PopulateMoveList(MoveList &move_list);

    void PopulateCaptureMoveList(MoveList &move_list);

    ChessBoard();

    ChessBoard(const std::string_view &FEN);

    ~ChessBoard() = default;

    Bitmap GetPieceBitmap(int piece) const { return piece_bitboard[piece]; }

    Bitmap GetOccupancyBitboard(Side side) const { return occupancy_bitboard[side]; };

    uint64_t GetPositionHashKey() const { return board_state.hash_key; }

    Piece At(int index) const { return (Piece) mailbox[index]; }

    Square GetEnpassantSquare() const { return (Square)board_state.en_passant; }

    CastlingRight GetCastlingRight() const { return (CastlingRight)board_state.castling_right; }

    Side CurrentSide() const { return (Side) board_state.side_to_move; }

    bool IsKingInCheck() const {
        return IsSquaredAttacked(piece_bitboard[board_state.side_to_move * 6 + WhiteKing].GetFirstLSBIndex(),
                                 opponent_side[board_state.side_to_move]);
    }

    void ClearBoard();

    static Piece CharToPieceIndex(char str);

    static char PieceToChar(int piece);

    static int GetFlippedSquare(int square) { return square ^ 56; } //Black magic ts oooooo

private:
    template<Side side>
    void PopulateMove(MoveList &move_list);

    template<Side side>
    void PopulateCaptureMove(MoveList &move_list);

public:
    static constexpr Side opponent_side[2] = {Black, White};
    static constexpr Piece opposite_piece[12] = {
        BlackPawn,
        BlackKnight,
        BlackBishop,
        BlackRook,
        BlackQueen,
        BlackKing,
        WhitePawn,
        WhiteKnight,
        WhiteBishop,
        WhiteRook,
        WhiteQueen,
        WhiteKing
    };

private:
    uint8_t mailbox[64];
    Bitmap piece_bitboard[12];
    Bitmap occupancy_bitboard[3];

    BoardState board_state;
    BoardState history[1028];
    int game_ply = 0;

    /**
     * p = Pawn
     * n = Knight
     * b = Bishop
     * r = Rook
     * q = Queen
     * k = Knight
     */

    static constexpr char piece_strings[13] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
    static constexpr uint8_t castling_right_mask[64] = {
#include "../Data/CastlingRightMatrix.dat"
    };

    static constexpr int forward_pawn_offset[2] = {-8, 8};
    static constexpr uint64_t promotion_zone[2] = {0xFFULL, 0xFFULL << (8 * 7)};
    static constexpr uint64_t pawn_start_rank[2] = {0xFFULL << (8 * 6), 0xFFULL << (8 * 1)};
};

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
    void AddScore(int index, int val) {
        score[index] = val;
    }

    int GetScore(int index) {
        return score[index];
    }

    MoveList() = default;
    ~MoveList() = default;

public:
    static constexpr int invalid_move = 0;

// private:
    static std::string FormatMoveToString(int index, int move);

private:
    int move_list[256];
    int score[256];
    int count = 0;
};


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

    temp = score[src];
    score[src] = score[tar];
    score[tar] = temp;
}


#endif //BITMAPMANIPULATOR_CHESSBOARD_H
