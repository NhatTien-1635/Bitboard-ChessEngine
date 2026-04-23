//
// Created by Hi on 4/22/2026.
//

#ifndef BITMAPMANIPULATOR_TRANSPOSITIONTABLE_H
#define BITMAPMANIPULATOR_TRANSPOSITIONTABLE_H

#include "ChessBoard.h"

class ChessBoard;

namespace PseudoRandomNumberGenerator {
    uint64_t GenerateRandomNumber64bit();
}

class TranspositionTable {
public:
    static void InitTable();

    static uint64_t GenerateKey(const ChessBoard &chess_board);

    static uint64_t GetKey() { return hash_key; }

    static void SetKey(uint64_t key) { hash_key = key; }

    static void UpdatePiece(uint8_t piece, uint8_t square) { hash_key ^= piece_key[piece][square]; }

    static void UpdateEnpassant(uint8_t square) { hash_key ^= enpassant_key[square]; }

    static void UpdateCastling(uint8_t rights) { hash_key ^= castle_key[rights]; }

    static void UpdateSide() { hash_key ^= side_key; }

    static void PrintTable();

public:
    TranspositionTable() = delete;

    ~TranspositionTable() = default;

public:
    struct Entry {
        int depth = 0;
        int score = 0;
    };

private:
    Entry hash_table[1 << 22];

    static uint64_t hash_key;

    static uint64_t piece_key[12][64];
    static uint64_t castle_key[16];
    static uint64_t enpassant_key[64];
    static uint64_t side_key;
};


#endif //BITMAPMANIPULATOR_TRANSPOSITIONTABLE_H
