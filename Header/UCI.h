//
// Created by Hi on 4/30/2026.
//

#ifndef BITMAPMANIPULATOR_UCI_H
#define BITMAPMANIPULATOR_UCI_H

#include "Terminal.h"
#include "Bitmap.h"
#include "MoveGenerator.h"
#include "MagicNumberGenerator.h"
#include "ChessBoard.h"
#include "PerformanceTest.h"
#include "Evaluator.h"
#include "Engine.h"
#include "TranspositionTable.h"

#include <string>
#include <sstream>
#include <iostream>
#include <thread>


class UCI {
public:
    static void InitializeEngine();

    void RunLoop();

public:
    UCI();

    ~UCI();

    void ParseCommand(const std::string &line);

private:
    void ParsePosition(const std::string &line);

    int ParseMove(const std::string &token);

public:
    void ParseGo(const std::string& line);

    void PrintBoard() const;

private:
    ChessBoard chess_board;
};


#endif //BITMAPMANIPULATOR_UCI_H
