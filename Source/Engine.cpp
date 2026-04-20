//
// Created by Hi on 4/18/2026.
//

#include "../Header/Engine.h"

#include "../Header/Evaluator.h"

int Engine::best_move = 0;

#ifdef PRINT_NODE
int Engine::node = 0;
#endif

int Engine::GetBestMove(ChessBoard &chess_board, int depth) {
    Negamax(-50000, 50000, chess_board, depth, 0);

#ifdef PRINT_NODE
    std::cout << "\nNode count: " << node << '\n';
#endif

    return best_move;
}

void Engine::PrintScoreMoves(const MoveList &move_list, const ChessBoard& chess_board) {
    for (int index = 0; index < move_list.GetMoveCount(); ++index) {
        int move = move_list.GetMove(index);
        std::cout << "Move: ";
        std::cout << MoveGenerator::SquareToString(MoveList::DecodeGetSourceSquare(move));
        std::cout << MoveGenerator::SquareToString(MoveList::DecodeGetTargetSquare(move));
        std::cout << ", Score: ";
        std::cout << ScoreMove(move, chess_board) << '\n';
    }
}

int Engine::Negamax(int alpha, int beta, ChessBoard &chess_board, int depth, int ply) {
    if (depth == 0) {
        return QuiescenceSearch(alpha, beta, chess_board, ply);
    }

#ifdef PRINT_NODE
    ++node;
#endif

    if (chess_board.IsKingInCheck()) {
        ++depth;
    }

    MoveList move_list;
    int best_move_so_far = 0;
    int old_alpha = alpha;
    int legal_move = 0;
    chess_board.PopulateMoveList(move_list);

    // while (move_list.GetMoveCount() > 0) {
    //     int move = SelectBestMove(move_list, chess_board);

    for (int index = 0; index < move_list.GetMoveCount(); ++index) {
        int move = move_list.GetMove(index);

        if (!chess_board.MakeMove(move)) {
            continue;
        }

        int score = -Negamax(-beta, -alpha, chess_board, depth - 1, ply + 1);
        ++legal_move;

        chess_board.UnmakeMove(move);

        //Fail-hard beta cutoff
        if (score >= beta) {
            //Node (move) fail high
            return beta;
        }

        if (score > alpha) {
            //PV node (move)
            alpha = score;

            if (ply == 0) {
                best_move_so_far = move;
            }
        }
    }

    //No legal move
    if (legal_move == 0) {
        if (chess_board.IsKingInCheck()) {
            return -50000 + ply;
        } else {
            return 0;
        }
    }

    if (old_alpha != alpha && ply == 0) {
        best_move = best_move_so_far;
    }

    //Node (move) fail low
    return alpha;
}

int Engine::QuiescenceSearch(int alpha, int beta, ChessBoard &chess_board, int ply) {
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

#ifdef PRINT_NODE
    ++node;
#endif

    MoveList move_list;
    chess_board.PopulateCaptureMoveList(move_list);

    // while (move_list.GetMoveCount() > 0) {
    // int move = SelectBestMove(move_list, chess_board);

    for (int index = 0; index < move_list.GetMoveCount(); ++index) {
        int move = move_list.GetMove(index);
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
        }
    }

    //Node (move) fail low
    return alpha;
}

int Engine::SelectBestMove(MoveList &move_list, const ChessBoard& chess_board) {
    int move_count = move_list.GetMoveCount();
    int best_index = 0;
    int max_score = -100000;

    for (int i = 0; i < move_count; ++i) {
        int current_score = ScoreMove(move_list.GetMove(i), chess_board);
        if (current_score > max_score) {
            max_score = current_score;
            best_index = i;
        }
    }

    move_list.Swap(best_index, move_count - 1);

    int move = move_list.GetMove(move_count - 1);
    move_list.PopBack();
    return move;
}

int Engine::ScoreMove(int encoded_move, const ChessBoard &chess_board) {
    if (MoveList::DecodeGetCapturePiece(encoded_move) == NoPiece) {
        return 0;
    }

    return mvv_laa_table[MoveList::DecodeGetPiece(encoded_move) % 6][MoveList::DecodeGetCapturePiece(encoded_move) % 6];
}
