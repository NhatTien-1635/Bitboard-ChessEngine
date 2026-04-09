//
// Created by Hi on 3/20/2026.
//

#ifndef BITMAPMANIPULATOR_MOVEGENERATOR_H
#define BITMAPMANIPULATOR_MOVEGENERATOR_H

#include <cstdint>
#include <string_view>

#include "Bitmap.h"

enum Side : uint8_t {
    White = 0,
    Black,
    Both
};

enum Square : uint8_t {
    a8 = 0, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    NoSquare = 64
};


class MoveGenerator {
public:
    static void InitGenerator();

    static Bitmap GetPawnAttack(int square, Side side);

    static Bitmap GetKnightAttack(int square);

    static Bitmap GetKingAttack(int square);

    static Bitmap GetBishopAttack(int square, Bitmap occupancy);

    static Bitmap GetRookAttack(int square, Bitmap occupancy);

    static Bitmap GetQueenAttack(int square, Bitmap occupancy);

    static std::string_view SquareToString(int square);

    MoveGenerator() = default;

    ~MoveGenerator() = default;

private:
    static void InitKingAttack();

    static void InitPawnAttack();

    static void InitKnightAttack();

    static void InitBishopAttack();

    static void InitRookAttack();

    static Bitmap GeneratePawnAttack(int square, Side side);

    static Bitmap GenerateKnightAttack(int square);

    static Bitmap GenerateKingAttack(int square);

    static Bitmap GenerateBishopRelevantOccupancy(int square);

    static Bitmap GenerateRookRelevantOccupancy(int square);

    static Bitmap GenerateBishopAttackOnRuntime(int square, uint64_t occupancy);

    static Bitmap GenerateRookAttackOnRuntime(int square, uint64_t occupancy);

    static Bitmap GenerateOccupancy(int index, Bitmap attack_mask);

private:
    static Bitmap pawn_attack[2][64];
    static Bitmap knight_attack[64];
    static Bitmap king_attack[64];

    static Bitmap relevant_bishop_occupancy[64];
    static Bitmap relevant_rook_occupancy[64];

    static Bitmap bishop_attack[64][512];
    static Bitmap rook_attack[64][4096];

    static const Bitmap bishop_occupancy_bit_count[64];
    static const Bitmap rook_occupancy_bit_count[64];

    static const Bitmap rook_magic_number[64];
    static const Bitmap bishop_magic_number[64];

    static Bitmap file_a_mask;
    static Bitmap file_h_mask;

    static Bitmap file_ab_mask;
    static Bitmap file_gh_mask;

    static constexpr std::string_view chess_square_string[64] = {
#include "../Data/ChessSquareString.dat"
    };
};
#endif //BITMAPMANIPULATOR_MOVEGENERATOR_H
