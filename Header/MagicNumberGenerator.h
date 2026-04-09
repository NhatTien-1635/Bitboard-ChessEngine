//
// Created by Hi on 3/22/2026.
//

#ifndef BITMAPMANIPULATOR_MAGICNUMBERGENERATOR_H
#define BITMAPMANIPULATOR_MAGICNUMBERGENERATOR_H

#include <cstdint>
#include <random>
#include <iostream>
#include <string.h>

#include "MoveGenerator.h"

class MagicNumberGenerator {
public:
    static uint64_t FindRookMagicNumber(int square);

    static uint64_t FindBishopMagicNumber(int square);

private:
    static uint64_t GenerateMagicNumberCandidate();

    static uint64_t GenerateRandomNumber64Bit();
};


#endif //BITMAPMANIPULATOR_MAGICNUMBERGENERATOR_H
