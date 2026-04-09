//
// Created by Hi on 3/20/2026.
//

#include "../Header/Terminal.h"

void Terminal::PrintBoard(uint64_t bitmap) {
    std::cout << "Bitmap value: " << bitmap << "\n";
    PrintLine();
    std::cout << std::setw(4) << "   a   b   c   d   e   f   g   h\n";
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            std::cout << (8 - (i / 8));
        }

        std::string output = ((1ULL << i) & bitmap) ? std::to_string(i) : ".";
        std::cout << std::setw(3) << output << ' ';
        if (i % 8 == 7) {
            std::cout << '\n';
        }
    }
    PrintLine();
}

void Terminal::PrintLine() {
    std::cout << "=================================\n";
}
