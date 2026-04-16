//
// Created by Hi on 4/14/2026.
//

#include "../Header/PerformanceTest.h"

uint64_t PerformanceTest::nodes = 0;
uint64_t PerformanceTest::castle_count = 0;
uint64_t PerformanceTest::capture_count = 0;
uint64_t PerformanceTest::enpassant_count = 0;
uint64_t PerformanceTest::promotion_count = 0;
std::chrono::duration<double> PerformanceTest::run_time;

void PerformanceTest::RunDriver(ChessBoard &board, int depth) {
   auto start_time = std::chrono::steady_clock::now();

   RunDriverHelper(board, depth);

   run_time = std::chrono::steady_clock::now() - start_time;
}

void PerformanceTest::PrintResult() {
#ifdef EXTRA_INFO
   std::cout << "Castling: : " << castle_count << '\n';
   std::cout << "Captures: " << capture_count << '\n';
   std::cout << "Promotion: " << promotion_count  << '\n';
   std::cout << "Enpassant: " << enpassant_count << '\n';
#endif

   std::cout << "Total node count: " << nodes << '\n';
   std::cout << "Run Time: " << run_time.count();
}

void PerformanceTest::RunDriverHelper(ChessBoard &board, int depth) {
#ifndef EXTRA_INFO
   MoveList move_list;
   board.PopulateMoveList(move_list);

   if (depth == 0) {
      ++nodes;
      return;
   }

#else
   if (depth == 1) {
      int legal_moves = 0;
      for (int i = 0; i < move_list.GetMoveCount(); ++i) {
         int encoded_move = move_list.GetMove(i);
         if (board.MakeQuietMove(move_list.GetMove(i))) {
            ++legal_moves;

            //Store extra info
            if (MoveList::DecodeGetCapturePiece(encoded_move) != NoPiece) {
               ++capture_count;
            }

            if (MoveList::DecodeGetCastlingFlag(encoded_move)) {
               ++castle_count;
            }

            if (MoveList::DecodeGetEnpassantFlag(encoded_move)) {
               ++enpassant_count;
            }

            if (MoveList::DecodeGetPromotedPiece(encoded_move) != NoPiece) {
               ++promotion_count;
            }

            board.UnmakeMove(move_list.GetMove(i));
         }
      }
      nodes += legal_moves;
      return;
   }
#endif

   for (int index = 0; index < move_list.GetMoveCount(); ++index) {

      int encoded_move = move_list.GetMove(index);
      if (!board.MakeQuietMove(encoded_move)) {
         continue;
      }
      RunDriverHelper(board, depth - 1);

      board.UnmakeMove(encoded_move);
   }
}