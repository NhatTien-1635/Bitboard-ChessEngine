//
// Created by Hi on 4/18/2026.
//

#include "../Header/Engine.h"

#include "../Header/Evaluator.h"

TranspositionTable Engine::hash_table;
int Engine::pv_length = 0;
int Engine::pv_table[Engine::max_ply]{};
int Engine::best_move = 0;
int Engine::lmr_table[64][64]{};

uint64_t Engine::node = 0;

#ifdef SPEED_TEST
std::chrono::duration<double> Engine::run_time;
#endif

#ifdef PRINT_DEBUG
int Engine::best_score = 0;
#endif

void Engine::InitTableLMR() {
    for (int depth = 0; depth < 64; ++depth) {
        for (int move_index = 0; move_index < 64; ++move_index) {
            if (depth > 0 && move_index > 0) {
                double reduction = 0.5 + ((std::log(depth) * std::log(move_index)) / helper_constant);
                lmr_table[depth][move_index] = (int) reduction;
            }
        }
    }
}

int Engine::GetBestMove(ChessBoard &chess_board, int depth) {
    std::cout << "Search best move position:\n";
    std::cout << "================================================\n";
    chess_board.PrintBoard();

#ifdef SPEED_TEST
    auto start_time = std::chrono::steady_clock::now();
#endif

    node = 0;
    pv_length = 0;
    Evaluator::ClearHistoryKillerMoveTable();
    hash_table.ClearTable();

    std::memset(pv_table, 0, sizeof(pv_table));
    for (int current_depth = 1; current_depth <= depth; ++current_depth) {
        Negamax(-Evaluator::infinity_score, Evaluator::infinity_score, chess_board, current_depth, 0);
        ExtractPV(chess_board, current_depth);

#ifdef PRINT_DEBUG
        std::cout << "================================================\n";
        std::cout << "Depth: " << current_depth << " | Score: " << best_score << '\n';
        std::cout << "Node count: " << node << '\n';
        std::cout << "PV moves: ";

        for (int index = 0; index < pv_length; ++index) {
            int move = pv_table[index];
            std::cout << MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(move)) <<
                    MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(move)) << ' ';
        }
        std::cout << '\n';
#endif
    }

#ifdef SPEED_TEST
    run_time = std::chrono::steady_clock::now() - start_time;
    std::cout << "Search time at depth " << depth << ": " << run_time.count() << '\n';
    std::cout << "Node count: " << node << '\n';
#endif

    return pv_table[0];
}

void Engine::PrintScoreMoves(const MoveList &move_list, const ChessBoard &chess_board) {
    for (int index = 0; index < move_list.GetMoveCount(); ++index) {
        int move = move_list.GetMove(index);
        std::cout << "Move: ";
        std::cout << MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(move));
        std::cout << MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(move));
        std::cout << ", Score: ";
        std::cout << Evaluator::ScoreMove(move, chess_board, 0) << '\n';
    }
}

int Engine::Negamax(int alpha, int beta, ChessBoard &chess_board, int depth, int ply) {
    int tt_move = 0;
    int val = hash_table.ReadEntry(chess_board.GetPositionHashKey(), alpha, beta, depth, ply, tt_move);
    HashFlag flag = AlphaFlag;
    if (ply > 0 && val != TranspositionTable::no_entry_value) {
        return val;
    }

    bool is_check = chess_board.IsKingInCheck();
    if (is_check) {
        ++depth;
    }

    if (depth <= 0) {
        return QuiescenceSearch(alpha, beta, chess_board, ply);
    }

    if (depth <= 2 && !is_check) {
        int static_eval = Evaluator::EvaluatePosition(chess_board);
        if (static_eval + 150 <= alpha) {
            return QuiescenceSearch(alpha, beta, chess_board, ply);
        }
    }

    ++node;

    //Null move pruning
    if (!is_check && depth >= full_depth_move_limit && ply > 0 && (beta - alpha == 1) && chess_board.HasMajorPieceLeft(
            chess_board.CurrentSide())) {
        chess_board.MakeNullMove();
        //Search with reduced depth to find beta cutoff
        int null_score = -Negamax(-beta, -beta + 1, chess_board, depth - 1 - 2, ply + 1);
        chess_board.UnmakeNullMove();

        if (null_score >= beta) {
            return beta;
        }
    }

    MoveList move_list;
    int best_move_so_far = tt_move;
    int old_alpha = alpha;
    int legal_move = 0;
    chess_board.PopulateMoveList(move_list);

    bool first_move = true;
    int move_index = 0;

    Evaluator::ScoreMoveList(move_list, chess_board, ply, tt_move);

    while (move_list.GetMoveCount() > 0) {
        int move = Evaluator::SelectBestMove(move_list);

        if (!chess_board.MakeMove(move)) {
            continue;
        }

        int score = 0;
        if (first_move) {
            score = -Negamax(-beta, -alpha, chess_board, depth - 1, ply + 1);
            first_move = false;
        } else {
            bool is_capture = (MoveList::DecodeGetCapturePiece(move) != NoPiece);
            bool is_promotion = (MoveList::DecodeGetPromotedPiece(move) != NoPiece);
            bool is_checking = chess_board.IsKingInCheck();
            int reduction = 0;

            //LMR condition
            if (depth >= reduction_limit && move_index >= full_depth_move_limit && !is_check && !is_capture && !
                is_promotion && !is_checking) {
                reduction = lmr_table[std::min(63, depth)][std::min(63, move_index)];
            }

            //Search with PVS + LMR
            score = -Negamax(-alpha - 1, -alpha, chess_board, depth - 1 - reduction, ply + 1);

            //Re-search with PVS
            if (reduction > 0 && score > alpha) {
                score = -Negamax(-alpha - 1, -alpha, chess_board, depth - 1, ply + 1);
            }

            //Standard re-search
            if (score > alpha && score < beta) {
                score = -Negamax(-beta, -alpha, chess_board, depth - 1, ply + 1);
            }
        }
        ++legal_move;

        chess_board.UnmakeMove(move);

        ++move_index;

        //Fail-hard beta cutoff
        if (score >= beta) {
            if (MoveList::DecodeGetCapturePiece(move) == NoPiece) {
                Evaluator::StoreKillerMove(move, ply);
            }

            hash_table.AddEntry(chess_board.GetPositionHashKey(), depth, ply, beta, BetaFlag, move);
            //Node (move) fail high
            return beta;
        }

        if (score > alpha) {
            if (MoveList::DecodeGetCapturePiece(move) == NoPiece) {
                Evaluator::UpdateHistoryMove(move, depth);
            }

            //PV node (move)
            alpha = score;
            flag = ExactFlag;

            best_move_so_far = move;
            if (ply == 0) {
#ifdef PRINT_DEBUG
                best_score = score;
#endif
            }
        }
    }

    //No legal move
    if (legal_move == 0) {
        if (chess_board.IsKingInCheck()) {
            return -Evaluator::infinity_score + ply;
        } else {
            return 0;
        }
    }

    if (old_alpha != alpha && ply == 0) {
        best_move = best_move_so_far;
    }

    hash_table.AddEntry(chess_board.GetPositionHashKey(), depth, ply, alpha, flag, best_move_so_far);
    //Node (move) fail low
    return alpha;
}

int Engine::QuiescenceSearch(int alpha, int beta, ChessBoard &chess_board, int ply) {
    int tt_move = 0;
    int val = hash_table.ReadEntry(chess_board.GetPositionHashKey(), alpha, beta, -1, ply, tt_move);
    HashFlag flag = AlphaFlag;
    if (ply > 0 && val != TranspositionTable::no_entry_value) {
        return val;
    }

    int evaluation = Evaluator::EvaluatePosition(chess_board);
    //Fail-hard beta cutoff
    if (evaluation >= beta) {
        //Node (move) fail high
        return beta;
    }

    if (evaluation > alpha) {
        //PV node (move)
        alpha = evaluation;
    }

    ++node;

    MoveList move_list;
    chess_board.PopulateCaptureMoveList(move_list);
    int best_move_so_far = tt_move;
    Evaluator::ScoreMoveList(move_list, chess_board, ply, tt_move);

    while (move_list.GetMoveCount() > 0) {
        int move = Evaluator::SelectBestMove(move_list);

        if (!chess_board.MakeCaptureMove(move)) {
            continue;
        }

        int score = -QuiescenceSearch(-beta, -alpha, chess_board, ply + 1);

        chess_board.UnmakeMove(move);

        //Fail-hard beta cutoff
        if (score >= beta) {
            //Node (move) fail high
            return beta;
        }

        if (score > alpha) {
            //PV node (move)
            alpha = score;
            flag = ExactFlag;
        }
    }

    hash_table.AddEntry(chess_board.GetPositionHashKey(), -1, ply, alpha, flag, best_move_so_far);
    //Node (move) fail low
    return alpha;
}

int Engine::ExtractPV(ChessBoard &chess_board, int depth) {
    int pv_count = 0;
    for (int index = 0; index < depth; ++index) {
        int tt_move = 0;
        hash_table.ReadEntry(chess_board.GetPositionHashKey(), -50000, 50000, -2, index, tt_move);

        if (tt_move == 0 || !chess_board.MakeMove(tt_move)) {
            break;
        }

        pv_table[pv_count++] = tt_move;
    }

    for (int index = pv_count - 1; index >= 0; --index) {
        chess_board.UnmakeMove(pv_table[index]);
    }

    pv_length = pv_count;
    return pv_count > 0 ? pv_table[0] : 0;
}
