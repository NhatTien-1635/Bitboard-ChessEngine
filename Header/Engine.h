//
// Created by Hi on 4/18/2026.
//

#ifndef BITMAPMANIPULATOR_ENGINE_H
#define BITMAPMANIPULATOR_ENGINE_H

#include "ChessBoard.h"
#include "TranspositionTable.h"

#include <atomic>
#include <cmath>
#include <chrono>
#include <complex>


//Debug macros
//==================================
// #define PRINT_DEBUG
// #define SPEED_TEST
//==================================

//Global Limits used by Engine and UCI
namespace Limits {
    alignas(64) inline std::atomic<bool> searching_flag{false};
    alignas(64) inline std::atomic<bool> stop_flag{false};
    alignas(64) inline std::atomic<uint64_t> node_count{0};

    inline int64_t start_time{0};
    inline int64_t time_limit{0};
    inline uint64_t max_node{0};

    inline bool use_time_management{false};
    inline bool infinite{false};

    inline int64_t GetTimeMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    inline bool ShouldStopSearching() {
        if ((node_count & 2047) == 0) {
            if (stop_flag) {
                return true;
            }

            if (use_time_management && !infinite) {
                if (GetTimeMs() >= (start_time + time_limit)) {
                    stop_flag = true;
                    return true;
                }
            }
        }
        return false;
    }

    inline std::string GetMoveString(int encoded_move) {
        std::string move;
        move += MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(encoded_move));
        move += MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(encoded_move));
        return move;
    }
}

class Engine {
public:
    static void InitTableLMR();

    static int GetBestMove(ChessBoard &chess_board, int depth);

    static void ClearHashTable() ;

public:
    Engine() = delete;

    ~Engine() = default;

    static void PrintScoreMoves(const MoveList &move_list, const ChessBoard &chess_board);

public:
    static constexpr int max_ply = 128;

private:
    static int Negamax(int alpha, int beta, ChessBoard &chess_board, int depth, int ply);

    static int QuiescenceSearch(int alpha, int beta, ChessBoard &chess_board, int ply);

    static int ExtractPV(ChessBoard &chess_board, int depth);

    static std::string GetPVString();

private:
    static constexpr int full_depth_move_limit = 4;
    static constexpr int reduction_limit = 3;

    static constexpr int window_value = 50;
    static constexpr int window_widening_value = 150;

private:
    static TranspositionTable hash_table;
    static int best_move;

    static int pv_table[max_ply];
    static int pv_length;

    //lmr_table[depth][move_index]
    static int lmr_table[64][64];
    static constexpr double helper_constant = 2.2;

#ifdef PRINT_DEBUG
    static int best_score;
#endif

#ifdef SPEED_TEST
    static std::chrono::duration<double> run_time;
#endif
};


#endif //BITMAPMANIPULATOR_ENGINE_H
