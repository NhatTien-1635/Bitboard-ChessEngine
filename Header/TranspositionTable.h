//
// Created by Hi on 4/22/2026.
//

#ifndef BITMAPMANIPULATOR_TRANSPOSITIONTABLE_H
#define BITMAPMANIPULATOR_TRANSPOSITIONTABLE_H

#include <algorithm>
#include <cstring>

#include "ChessBoard.h"
#include <vector>
#include <random>

class ChessBoard;

#define DEBUG_PSEUDO_RANDOM_GENERATOR

namespace PseudoRandomNumberGenerator {
    uint64_t GenerateRandomNumber64bit();
}

enum HashFlag : uint8_t {
    AlphaFlag = 0,
    BetaFlag,
    ExactFlag,
    NoFlag
};

struct Entry {
    uint64_t hash_key = 0;
    int depth = 0;
    int score = 0;
    HashFlag flag = NoFlag;
    int encoded_best_move = 0;
};

class TranspositionTable {
public:
    static void InitHashTable();

    static uint64_t GenerateKey(const ChessBoard &chess_board);

    static uint64_t GetKey() { return hash_key; }

    static void SetKey(uint64_t key) { hash_key = key; }

    static void UpdatePiece(uint8_t piece, uint8_t square) { hash_key ^= piece_key[piece][square]; }

    static void UpdateEnpassant(uint8_t square) { hash_key ^= enpassant_key[square]; }

    static void UpdateCastling(uint8_t rights) { hash_key ^= castle_key[rights]; }

    static void UpdateSide() { hash_key ^= side_key; }

    static void PrintTable();

    int ReadEntry(uint64_t key, int alpha, int beta, int depth, int ply, int &return_best_move) const;

    void AddEntry(uint64_t key, int depth, int ply, int score, HashFlag flag, int encoded_best_move);

    void RemoveEntry(uint64_t key);

    void ClearTable() { std::fill(std::begin(hash_table), std::end(hash_table), Entry{}); }

public:
    TranspositionTable() : hash_table(hash_table_size) { ClearTable(); };

    ~TranspositionTable() = default;

public:
    static constexpr int no_entry_value = INT_MIN;

private:
    static constexpr size_t hash_table_size = (1 << 22);
    std::vector<Entry> hash_table;

    static uint64_t hash_key;

    static uint64_t piece_key[12][64];
    static uint64_t castle_key[16];
    static uint64_t enpassant_key[64];
    static uint64_t side_key;
};


#endif //BITMAPMANIPULATOR_TRANSPOSITIONTABLE_H
