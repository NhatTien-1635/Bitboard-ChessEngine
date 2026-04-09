//
// Created by Hi on 3/23/2026.
//

#ifndef BITMAPMANIPULATOR_CHESSBOARD_H
#define BITMAPMANIPULATOR_CHESSBOARD_H

#include <iostream>
#include <iomanip>
#include <string.h>

#include "MoveGenerator.h"
#include "MoveList.h"

/**
 *  FEN Position
 *  Reference: https://www.chessprogramming.org/Forsyth-Edwards_Notation
 */

//Debug test
#define FEN_EMPTY_BOARD "8/8/8/8/8/8/8/8 w - - "
#define FEN_STARTING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_TRICKY_POSITION "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define FEN_KILLER_POSITION "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define FEN_CMK_POSITION "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

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
};

class ChessBoard {
public:
    void PrintBoard() const;

    void PrintAttack(Side side) const;

    void ParsePositionFromFEN(std::string_view position);

    bool MakeQuietMove(int encoded_move);
    bool MakeCaptureMove(int encoded_move);

    void UnmakeMove(int encoded_move);
    void ReserveGameState();
    void RestoreGameState();

    //Return if the square is attacked by the opposite color
    bool IsSquaredAttacked(uint8_t square, uint8_t attacker) const;

    ChessBoard();

    ChessBoard(const std::string_view &FEN);

    ~ChessBoard() = default;

    //Private later
public:
    void PlacePiece(int square, Piece piece);

    void RemovePiece(int square);

    Bitmap GetPieceBitmap(int piece) const;

    Piece At(int index) const;

    void ClearBoard();

    void PopulateMoveList(MoveList& move_list);

    // private:
    static Piece CharToPieceIndex(char str);

    static char PieceToChar(int piece);

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
};


#endif //BITMAPMANIPULATOR_CHESSBOARD_H
