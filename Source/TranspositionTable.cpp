//
// Created by Hi on 4/22/2026.
//

#include "../Header/TranspositionTable.h"

uint64_t TranspositionTable::hash_key;

uint64_t TranspositionTable::piece_key[12][64];
uint64_t TranspositionTable::castle_key[16];
uint64_t TranspositionTable::enpassant_key[64];
uint64_t TranspositionTable::side_key;

namespace {
    int random_number = 0x8DC672B1;

    int GetRandomNumber() {
        random_number ^= random_number << 13;
        random_number ^= random_number >> 17;
        random_number ^= random_number << 5;
        return random_number;
    }
}

namespace PseudoRandomNumberGenerator {
    uint64_t GenerateRandomNumber64bit() {
#ifdef DEBUG_PSEUDO_RANDOM_GENERATOR
        uint64_t first = GetRandomNumber();
        uint64_t second = GetRandomNumber();
        uint64_t third = GetRandomNumber();
        uint64_t forth = GetRandomNumber();

        return ((forth & 0xffff) << 48) |
               ((third & 0xffff) << 32) |
               ((second & 0xffff) << 16) |
               (first & 0xffff);

#else
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<std::uint64_t> dis;
        return dis(gen);
#endif
    }
}

void TranspositionTable::InitHashTable() {
    for (int piece = 0; piece < 12; ++piece) {
        for (int square = 0; square < 64; ++square) {
            piece_key[piece][square] = PseudoRandomNumberGenerator::GenerateRandomNumber64bit();
        }
    }

    for (int square = 0; square < 64; ++square) {
        enpassant_key[square] = PseudoRandomNumberGenerator::GenerateRandomNumber64bit();
    }

    for (int right = 0; right < 16; ++right) {
        castle_key[right] = PseudoRandomNumberGenerator::GenerateRandomNumber64bit();
    }
    castle_key[0] = 0;

    side_key = PseudoRandomNumberGenerator::GenerateRandomNumber64bit();
}

uint64_t TranspositionTable::GenerateKey(const ChessBoard &chess_board) {
    uint64_t key = 0;

    if (chess_board.CurrentSide() == Black) {
        key ^= side_key;
    }

    for (int square = 0; square < 64; ++square) {
        if (chess_board.At(square) != NoPiece) {
            key ^= piece_key[chess_board.At(square)][square];
        }
    }

    if (chess_board.GetEnpassantSquare() != NoSquare) {
        key ^= enpassant_key[chess_board.GetEnpassantSquare()];
    }

    key ^= castle_key[chess_board.GetCastlingRight()];

    return key;
}

void TranspositionTable::PrintTable() {
    std::cout << "Piece keys:\n";
    for (int piece = 0; piece < 12; ++piece) {
        for (int square = 0; square < 64; ++square) {
            std::cout << piece_key[piece][square] << '\n';
        }
    }

    std::cout << "Enpassant keys:\n";
    for (int square = 0; square < 64; ++square) {
        std::cout << enpassant_key[square] << '\n';
    }

    std::cout << "Castle keys:\n";
    for (int right = 0; right < 16; ++right) {
        std::cout << castle_key[right] << '\n';
    }

    std::cout << "Side key: ";
    std::cout << side_key;
}

int TranspositionTable::ReadEntry(uint64_t key, int alpha, int beta, int depth, int ply, int &return_best_move) const {
    size_t index = (key & (hash_table_size - 1));

    if (key == hash_table[index].hash_key) {
        int score = hash_table[index].score;
        if (score > 49000) score -= ply;
        if (score < -49000) score += ply;

        return_best_move = hash_table[index].encoded_best_move;
        if (hash_table[index].depth >= depth) {
            if (hash_table[index].flag == ExactFlag) {
                return score;
            }

            if ((hash_table[index].flag == AlphaFlag) && (score <= alpha)) {
                return alpha;
            }

            if ((hash_table[index].flag == BetaFlag) && (score >= beta)) {
                return beta;
            }
        }
    }

    return no_entry_value;
}

void TranspositionTable::AddEntry(uint64_t key, int depth, int ply, int score, HashFlag flag, int encoded_best_move) {
    size_t index = (key & (hash_table_size - 1));

    if (score > 49000) score += ply;
    if (score < -49000) score -= ply;

    //Depth-preferred overwriting
    if (hash_table[index].hash_key == 0 || hash_table[index].hash_key == key || hash_table[index].depth < depth) {
        hash_table[index].hash_key = key;
        hash_table[index].depth = depth;
        hash_table[index].score = score;
        hash_table[index].flag = flag;
        hash_table[index].encoded_best_move = encoded_best_move;
    }
}

void TranspositionTable::RemoveEntry(uint64_t key) {
    size_t index = (key & (hash_table_size - 1));

    hash_table[index].hash_key = 0;
    hash_table[index].depth = 0;
    hash_table[index].score = 0;
    hash_table[index].flag = NoFlag;
    hash_table[index].encoded_best_move = 0;
}
