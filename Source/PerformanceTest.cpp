//
// Created by Hi on 4/14/2026.
//

#include "../Header/PerformanceTest.h"

uint64_t PerformanceTest::nodes = 0;
std::chrono::duration<double> PerformanceTest::run_time;

void PerformanceTest::RunDriver(ChessBoard &board, int depth) {
   auto start_time = std::chrono::steady_clock::now();

   RunDriverHelper(board, depth);

   run_time = std::chrono::steady_clock::now() - start_time;
}

void PerformanceTest::PrintResult() {
   std::cout << "Total node count: " << nodes << '\n';
   std::cout << "Run Time: " << run_time.count();
}

void PerformanceTest::RunDriverHelper(ChessBoard &board, int depth) {
   if (depth == 0) {
      ++nodes;
      return;
   }

   MoveList move_list;
   board.PopulateMoveList(move_list);
   for (int index = 0; index < move_list.GetMoveCount(); ++index) {
      int encoded_move = move_list.GetMove(index);

      if (!board.MakeQuietMove(encoded_move)) {
         continue;
      }

      RunDriverHelper(board, depth - 1);

      board.UnmakeMove(encoded_move);
   }
}
