//
// Created by Hi on 3/20/2026.
//

#ifndef BITMAPMANIPULATOR_TERMINAL_H
#define BITMAPMANIPULATOR_TERMINAL_H

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

class Terminal {
public:
    static void PrintBoard(uint64_t);
    static void PrintLine();

    Terminal() = default;
    ~Terminal() = default;

};


#endif //BITMAPMANIPULATOR_TERMINAL_H