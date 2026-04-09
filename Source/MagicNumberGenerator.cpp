//
// Created by Hi on 3/22/2026.
//

#include "../Header/MagicNumberGenerator.h"

uint64_t MagicNumberGenerator::FindRookMagicNumber(int square) {
    Bitmap relevant_mask = MoveGenerator::GenerateRookRelevantOccupancy(square);
    int bit_count = relevant_mask.GetBitCount();

    Bitmap attack[4096];
    Bitmap occupancy[4096];
    Bitmap used_attack[4096];

    int occupancy_count = (1 << bit_count);
    for (int index = 0; index < occupancy_count; ++index) {
        occupancy[index] = MoveGenerator::GenerateOccupancy(index, relevant_mask);

        attack[index] = MoveGenerator::GenerateRookAttackOnRuntime(square, occupancy[index]);
    }

    for (int attempt = 0; attempt < 10000000; ++attempt) {
        uint64_t candidate = GenerateMagicNumberCandidate();

        if (((candidate * relevant_mask) & 0xFF00000000000000) < 6) {
            continue;
        }

        memset(used_attack, 0, sizeof(used_attack));

        bool fail = false;
        for (int index = 0; index < occupancy_count; ++index) {
            int magic_index = (int) ((candidate * occupancy[index]) >> (64 - bit_count));

            if (used_attack[magic_index] == 0ULL) {
                used_attack[magic_index] = attack[index];
            } else if (used_attack[magic_index] != attack[index]) {
                fail = true;
                break;
            }
        }

        if (!fail) {
            return candidate;
        }
    }

    std::cout << "Finding number failed!\n";
    return 0ULL;
}

uint64_t MagicNumberGenerator::FindBishopMagicNumber(int square) {
    Bitmap relevant_mask = MoveGenerator::GenerateBishopRelevantOccupancy(square);
    int bit_count = relevant_mask.GetBitCount();

    Bitmap attack[512];
    Bitmap occupancy[512];
    Bitmap used_attack[512];

    int occupancy_count = (1 << bit_count);
    for (int index = 0; index < occupancy_count; ++index) {
        occupancy[index] = MoveGenerator::GenerateOccupancy(index, relevant_mask);

        attack[index] = MoveGenerator::GenerateBishopAttackOnRuntime(square, occupancy[index]);
    }

    for (int attempt = 0; attempt < 100000000; ++attempt) {
        uint64_t candidate = GenerateMagicNumberCandidate();

        if (((candidate * relevant_mask) & 0xFF00000000000000) < 6) {
            continue;
        }

        memset(used_attack, 0, sizeof(used_attack));

        bool fail = false;
        for (int index = 0; index < occupancy_count; ++index) {
            int magic_index = (int) ((candidate * occupancy[index]) >> (64 - bit_count));

            if (used_attack[magic_index] == 0ULL) {
                used_attack[magic_index] = attack[index];
            } else if (used_attack[magic_index] != attack[index]) {
                fail = true;
                break;
            }
        }

        if (!fail) {
            return candidate;
        }
    }

    std::cout << "Finding number failed!\n";
    return 0ULL;
}

uint64_t MagicNumberGenerator::GenerateMagicNumberCandidate() {
    return GenerateRandomNumber64Bit() & GenerateRandomNumber64Bit() & GenerateRandomNumber64Bit();
}

uint64_t MagicNumberGenerator::GenerateRandomNumber64Bit() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<std::uint64_t> dis;

    return dis(gen);
}
