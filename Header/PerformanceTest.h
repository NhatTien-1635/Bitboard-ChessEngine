//
// Created by Hi on 4/14/2026.
//

#ifndef BITMAPMANIPULATOR_PERFORMANCETEST_H
#define BITMAPMANIPULATOR_PERFORMANCETEST_H

#include <chrono>
#include <iostream>

#include "ChessBoard.h"

class PerformanceTest {
public:
    PerformanceTest() = default;

    ~PerformanceTest() = default;

    static void RunDriver(ChessBoard &board, int depth);

    static void PrintResult();

private:
    static void RunDriverHelper(ChessBoard &board, int depth);

    static uint64_t nodes;
    static uint64_t castle_count;
    static uint64_t capture_count;
    static uint64_t enpassant_count;
    static uint64_t promotion_count;

    static std::chrono::duration<double> run_time;
};


#endif //BITMAPMANIPULATOR_PERFORMANCETEST_H
