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

    inline static Bitmap GetAttack(uint8_t piece, int square, Side side, Bitmap occupancy);

    inline static Bitmap GetPawnAttack(int square, Side side);

    inline static Bitmap GetKnightAttack(int square);

    inline static Bitmap GetKingAttack(int square);

    inline static Bitmap GetBishopAttack(int square, Bitmap occupancy);

    inline static Bitmap GetRookAttack(int square, Bitmap occupancy);

    inline static Bitmap GetQueenAttack(int square, Bitmap occupancy);

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

Bitmap MoveGenerator::GetAttack(uint8_t piece, int square, Side side, Bitmap occupancy) {
    switch (piece) {
        case 0: return GetPawnAttack(square, side);
        case 1: return GetKnightAttack(square);
        case 2: return GetBishopAttack(square, occupancy);
        case 3: return GetRookAttack(square, occupancy);
        case 4: return GetQueenAttack(square, occupancy);
        case 5: return GetKingAttack(square);
        default:
            return Bitmap();
    }
    return Bitmap();
}

Bitmap MoveGenerator::GetPawnAttack(int square, Side side) {
    if (square == 255 || square == -1) {
        return Bitmap();
    }

    return pawn_attack[side][square];
}

Bitmap MoveGenerator::GetKnightAttack(int square) {
    if (square == 255 || square == -1) {
        return Bitmap();
    }

    return knight_attack[square];
}

Bitmap MoveGenerator::GetKingAttack(int square) {
    if (square == 255 || square == -1) {
        return Bitmap();
    }

    return king_attack[square];
}

Bitmap MoveGenerator::GetBishopAttack(int square, Bitmap occupancy) {
    if (square == 255 || square == -1) {
        return Bitmap();
    }

    occupancy &= relevant_bishop_occupancy[square];
    occupancy *= bishop_magic_number[square];
    occupancy >>= (64 - bishop_occupancy_bit_count[square]);

    return bishop_attack[square][occupancy];
}

Bitmap MoveGenerator::GetRookAttack(int square, Bitmap occupancy) {
    if (square == 255 || square == -1) {
        return Bitmap();
    }

    occupancy &= relevant_rook_occupancy[square];
    occupancy *= rook_magic_number[square];
    occupancy >>= (64 - rook_occupancy_bit_count[square]);

    return rook_attack[square][occupancy];
}

Bitmap MoveGenerator::GetQueenAttack(int square, Bitmap occupancy) {
    return (GetRookAttack(square, occupancy) | GetBishopAttack(square, occupancy));
}

#endif //BITMAPMANIPULATOR_MOVEGENERATOR_H
