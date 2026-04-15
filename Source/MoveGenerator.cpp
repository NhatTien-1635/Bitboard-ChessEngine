//
// Created by Hi on 3/20/2026.
//

#include "../Header/MoveGenerator.h"

Bitmap MoveGenerator::file_a_mask;
Bitmap MoveGenerator::file_h_mask;
Bitmap MoveGenerator::file_ab_mask;
Bitmap MoveGenerator::file_gh_mask;

Bitmap MoveGenerator::pawn_attack[2][64];
Bitmap MoveGenerator::knight_attack[64];
Bitmap MoveGenerator::king_attack[64];

Bitmap MoveGenerator::relevant_bishop_occupancy[64];
Bitmap MoveGenerator::relevant_rook_occupancy[64];

Bitmap MoveGenerator::bishop_attack[64][512];
Bitmap MoveGenerator::rook_attack[64][4096];

const Bitmap MoveGenerator::bishop_occupancy_bit_count[64]{
#include "../Data/BishopOccupancyBitCount.dat"
};

const Bitmap MoveGenerator::rook_occupancy_bit_count[64]{
#include "../Data/RookOccupancyBitCount.dat"
};

const Bitmap MoveGenerator::rook_magic_number[64]{
#include "../Data/RookMagicNumber.dat"
};

const Bitmap MoveGenerator::bishop_magic_number[64]{
#include "../Data/BishopMagicNumber.dat"
};

void MoveGenerator::InitGenerator() {
    file_a_mask = Bitmap::GetColumnMask(0);
    file_h_mask = Bitmap::GetColumnMask(7);
    file_ab_mask = Bitmap::GetColumnMask(0) | Bitmap::GetColumnMask(1);
    file_gh_mask = Bitmap::GetColumnMask(6) | Bitmap::GetColumnMask(7);

    InitPawnAttack();
    InitKnightAttack();
    InitKingAttack();
    InitBishopAttack();
    InitRookAttack();
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

void MoveGenerator::InitBishopAttack() {
    for (int square = 0; square < 64; ++square) {
        relevant_bishop_occupancy[square] = GenerateBishopRelevantOccupancy(square);

        Bitmap attack_mask = relevant_bishop_occupancy[square];

        int relevant_bits = bishop_occupancy_bit_count[square];

        int occupancy_indices = (1 << relevant_bits);
        for (int index = 0; index < occupancy_indices; ++index) {
            Bitmap occupancy = GenerateOccupancy(index, attack_mask);

            int magic_index = (occupancy * bishop_magic_number[square]) >> (64 - relevant_bits);
            bishop_attack[square][magic_index] = GenerateBishopAttackOnRuntime(square, occupancy);
        }
    }
}

void MoveGenerator::InitRookAttack() {
    for (int square = 0; square < 64; ++square) {
        relevant_rook_occupancy[square] = GenerateRookRelevantOccupancy(square);

        Bitmap attack_mask = relevant_rook_occupancy[square];

        int relevant_bits = rook_occupancy_bit_count[square];

        int occupancy_indices = (1 << relevant_bits);
        for (int index = 0; index < occupancy_indices; ++index) {
            Bitmap occupancy = GenerateOccupancy(index, attack_mask);

            int magic_index = (occupancy * rook_magic_number[square]) >> (64 - relevant_bits);
            rook_attack[square][magic_index] = GenerateRookAttackOnRuntime(square, occupancy);
        }
    }
}

Bitmap MoveGenerator::GenerateBishopRelevantOccupancy(int square) {
    int row = square / 8;
    int col = square % 8;

    Bitmap bitmap = 0;

    for (int i = row + 1, j = col + 1; i <= 6 && j <= 6; ++i, ++j) {
        bitmap |= 1ULL << (i * 8 + j);
    }

    for (int i = row + 1, j = col - 1; i <= 6 && j >= 1; ++i, --j) {
        bitmap |= 1ULL << (i * 8 + j);
    }

    for (int i = row - 1, j = col + 1; i >= 1 && j <= 6; --i, ++j) {
        bitmap |= 1ULL << (i * 8 + j);
    }

    for (int i = row - 1, j = col - 1; i >= 1 && j >= 1; --i, --j) {
        bitmap |= 1ULL << (i * 8 + j);
    }
    return bitmap;
}

Bitmap MoveGenerator::GenerateRookRelevantOccupancy(int square) {
    int row = square / 8;
    int col = square % 8;

    Bitmap bitmap = 0;

    for (int i = row + 1; i <= 6; ++i) {
        bitmap |= 1ULL << (i * 8 + col);
    }

    for (int i = row - 1; i >= 1; --i) {
        bitmap |= 1ULL << (i * 8 + col);
    }

    for (int i = col + 1; i <= 6; ++i) {
        bitmap |= 1ULL << (row * 8 + i);
    }

    for (int i = col - 1; i >= 1; --i) {
        bitmap |= 1ULL << (row * 8 + i);
    }

    return bitmap;
}

Bitmap MoveGenerator::GenerateBishopAttackOnRuntime(int square, uint64_t occupancy) {
    int row = square / 8;
    int col = square % 8;

    Bitmap bitmap = 0;

    for (int i = row + 1, j = col + 1; i <= 7 && j <= 7; ++i, ++j) {
        bitmap |= 1ULL << (i * 8 + j);
        if (1ULL << (i * 8 + j) & occupancy) {
            break;
        }
    }

    for (int i = row + 1, j = col - 1; i <= 7 && j >= 0; ++i, --j) {
        bitmap |= 1ULL << (i * 8 + j);
        if (1ULL << (i * 8 + j) & occupancy) {
            break;
        }
    }

    for (int i = row - 1, j = col + 1; i >= 0 && j <= 7; --i, ++j) {
        bitmap |= 1ULL << (i * 8 + j);
        if (1ULL << (i * 8 + j) & occupancy) {
            break;
        }
    }

    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j) {
        bitmap |= 1ULL << (i * 8 + j);
        if (1ULL << (i * 8 + j) & occupancy) {
            break;
        }
    }
    return bitmap;
}

Bitmap MoveGenerator::GenerateRookAttackOnRuntime(int square, uint64_t occupancy) {
    int row = square / 8;
    int col = square % 8;

    Bitmap bitmap = 0;

    for (int i = row + 1; i <= 7; ++i) {
        bitmap |= 1ULL << (i * 8 + col);
        if (1ULL << (i * 8 + col) & occupancy) {
            break;
        }
    }

    for (int i = row - 1; i >= 0; --i) {
        bitmap |= 1ULL << (i * 8 + col);
        if (1ULL << (i * 8 + col) & occupancy) {
            break;
        }
    }

    for (int i = col + 1; i <= 7; ++i) {
        bitmap |= 1ULL << (row * 8 + i);
        if (1ULL << (row * 8 + i) & occupancy) {
            break;
        }
    }

    for (int i = col - 1; i >= 0; --i) {
        bitmap |= 1ULL << (row * 8 + i);
        if (1ULL << (row * 8 + i) & occupancy) {
            break;
        }
    }

    return bitmap;
}

Bitmap MoveGenerator::GenerateOccupancy(int index, Bitmap attack_mask) {
    Bitmap occupancy = 0ULL;
    int bit_count = attack_mask.GetBitCount();

    for (int count = 0; count < bit_count; ++count) {
        int first_bit_index = attack_mask.GetFirstLSBIndex();

        attack_mask.RemoveBit(first_bit_index);

        if (index & (1 << count)) {
            occupancy |= (1ULL << first_bit_index);
        }
    }
    return occupancy;
}

void MoveGenerator::InitPawnAttack() {
    for (int square = 0; square < 64; ++square) {
        pawn_attack[White][square] = GeneratePawnAttack(square, White);
        pawn_attack[Black][square] = GeneratePawnAttack(square, Black);
    }
}

std::string_view MoveGenerator::SquareToString(int square) {
    return chess_square_string[square];
}

void MoveGenerator::InitKnightAttack() {
    for (int square = 0; square < 64; ++square) {
        knight_attack[square] = GenerateKnightAttack(square);
    }
}

void MoveGenerator::InitKingAttack() {
    for (int square = 0; square < 64; ++square) {
        king_attack[square] = GenerateKingAttack(square);
    }
}

Bitmap MoveGenerator::GeneratePawnAttack(int square, Side side) {
    Bitmap bitmap = 1ULL << square;
    Bitmap attack = 0;

    if (side == White) {
        if (bitmap & ~file_h_mask) attack |= (bitmap >> 7);
        if (bitmap & ~file_a_mask) attack |= (bitmap >> 9);
    } else {
        if (bitmap & ~file_a_mask) attack |= (bitmap << 7);
        if (bitmap & ~file_h_mask) attack |= (bitmap << 9);
    }

    return attack;
}

Bitmap MoveGenerator::GenerateKnightAttack(int square) {
    Bitmap bitmap = 1ULL << square;
    Bitmap attack = 0;

    if ((bitmap >> 17) & ~file_h_mask) attack |= (bitmap >> 17);
    if ((bitmap >> 15) & ~file_a_mask) attack |= (bitmap >> 15);
    if ((bitmap >> 10) & ~file_gh_mask) attack |= (bitmap >> 10);
    if ((bitmap >> 6) & ~file_ab_mask) attack |= (bitmap >> 6);

    if ((bitmap << 17) & ~file_a_mask) attack |= (bitmap << 17);
    if ((bitmap << 15) & ~file_h_mask) attack |= (bitmap << 15);
    if ((bitmap << 10) & ~file_ab_mask) attack |= (bitmap << 10);
    if ((bitmap << 6) & ~file_gh_mask) attack |= (bitmap << 6);

    return attack;
}

Bitmap MoveGenerator::GenerateKingAttack(int square) {
    Bitmap bitmap = 1ULL << square;
    Bitmap attack = 0;

    if ((bitmap << 1) & ~file_a_mask) attack |= (bitmap << 1);
    attack |= (bitmap << 8);
    if ((bitmap << 9) & ~file_a_mask) attack |= (bitmap << 9);
    if ((bitmap << 7) & ~file_h_mask) attack |= (bitmap << 7);

    if ((bitmap >> 1) & ~file_h_mask) attack |= (bitmap >> 1);
    attack |= (bitmap >> 8);
    if ((bitmap >> 9) & ~file_h_mask) attack |= (bitmap >> 9);
    if ((bitmap >> 7) & ~file_a_mask) attack |= (bitmap >> 7);

    return attack;
}
