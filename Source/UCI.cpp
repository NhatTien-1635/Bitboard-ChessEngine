//
// Created by Hi on 4/30/2026.
//

#include "../Header/UCI.h"

void UCI::InitializeEngine() {
    MoveGenerator::InitGenerator();
    Evaluator::InitializeEvaluationTable();
    TranspositionTable::InitHashTable();
    Engine::InitTableLMR();
}

void UCI::RunLoop() {
    std::string line;
    while (std::getline(std::cin, line)) {
        ParseCommand(line);

        if (line == "quit") {
            break;
        }
    }
}

void UCI::ParseCommand(const std::string &line) {
    if (line.find("position") != std::string::npos) {
        ParsePosition(line);
    }

    if (line.find("go") != std::string::npos){
        ParseGo(line);
    }
}

void UCI::ParsePosition(const std::string &line) {
    std::stringstream ss(line);
    std::string token;

    ss >> token; //"position"
    ss >> token; //fen/startpos

    if (token == "startpos") {
        chess_board.ParsePositionFromFEN(FEN_STARTING_POSITION);
    }

    if (token == "fen") {
        std::string fen;
        for (int i = 0; i < 6; ++i) {
            if (!(ss >> token) || token == "moves") break;
            fen += (i == 0 ? "" : " ") + token;
        }
        chess_board.ParsePositionFromFEN(fen);
    }

    //We reached the end
    if (!(ss >> token) || token != "moves") {
        return;
    }

    std::string move_token;
    while (ss >> move_token) {
        int move = ParseMove(move_token);
        chess_board.MakeMove(move);
    }
}

UCI::UCI() {
    InitializeEngine();
    chess_board.ParsePositionFromFEN(FEN_STARTING_POSITION);
}

UCI::~UCI() {
}

//Parse move from input (ex: e2e4, e7e8q,...)
int UCI::ParseMove(const std::string &token) {
    int source_square = (int) (token.at(0) - 'a') + (8 - (int) (token.at(1) - '0')) * 8;
    int target_square = (int) (token.at(2) - 'a') + (8 - (int) (token.at(3) - '0')) * 8;

    MoveList move_list;
    chess_board.PopulateMoveList(move_list);

    int move = MoveList::invalid_move;
    int move_count = move_list.GetMoveCount();
    for (int index = 0; index < move_count; ++index) {
        move = move_list.GetMove(index);

        if (MoveList::DecodeGetSourceSquare(move) != source_square || MoveList::DecodeGetTargetSquare(move) !=
            target_square) {
            continue;
        }

        //No promotion
        Piece promotion_move = MoveList::DecodeGetPromotedPiece(move);
        if (token.length() == 4 && promotion_move == NoPiece) {
            return move;
        }

        //Checking promotion
        char promoted_piece = token.at(4);

        //Promoted to a knight
        if (promoted_piece == 'n' && (promotion_move == WhiteKnight || promotion_move == BlackKnight)) {
            return move;
        }

        //Promoted to a bishop
        if (promoted_piece == 'b' && (promotion_move == WhiteBishop || promotion_move == BlackBishop)) {
            return move;
        }

        //Promoted to a rook
        if (promoted_piece == 'r' && (promotion_move == WhiteRook || promotion_move == BlackRook)) {
            return move;
        }

        //Promoted to a queen
        if (promoted_piece == 'q' && (promotion_move == WhiteQueen || promotion_move == BlackQueen)) {
            return move;
        }
    }

    //Somehow we got here :D?
    throw std::logic_error("ParseMove -> token input is an illegal move!");
}

void UCI::ParseGo(const std::string line) {

}

void UCI::PrintBoard() const {
    chess_board.PrintBoard();
}
