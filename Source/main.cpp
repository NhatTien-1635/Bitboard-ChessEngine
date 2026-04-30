#include "../Header/UCI.h"

#define FEN_TEST_POSITION_1 "7k/p7/Pp5p/1Pp3pP/2Pp1pP1/3PpP2/4P3/K7 w - - 0 1"
#define FEN_TEST_POSITION_2 "8/8/8/8/8/8/8/8 w - - 0 0"

int main() {
    bool debug = true;

    if (debug) {
        UCI uci;

        uci.ParseCommand("position startpos moves e2e4");
        uci.PrintBoard();

    } else {
        UCI uci;
        uci.RunLoop();
    }


    return 0;
}
